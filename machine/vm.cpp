#include "vm.hpp"
#include "state.hpp"
#include "memory.hpp"
#include "machine_code.hpp"
#include "environment.hpp"
#include "thread_phase.hpp"

#include "memory/gc.hpp"

#include "object_utils.hpp"

#include "class/array.hpp"
#include "class/array.hpp"
#include "class/call_site.hpp"
#include "class/channel.hpp"
#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/fiber.hpp"
#include "class/fixnum.hpp"
#include "class/jit.hpp"
#include "class/list.hpp"
#include "class/location.hpp"
#include "class/lookup_table.hpp"
#include "class/native_method.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/system.hpp"
#include "class/thread.hpp"
#include "class/tuple.hpp"
#include "class/variable_scope.hpp"

#include "config_parser.hpp"
#include "config.h"

#include "call_frame.hpp"
#include "signal.hpp"
#include "configuration.hpp"
#include "helpers.hpp"
#include "park.hpp"

#include "util/thread.hpp"

#include "diagnostics/timing.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>
#include <signal.h>
#ifndef RBX_WINDOWS
#include <sys/resource.h>
#endif

// Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) globals().whatever.get()
#define GO(whatever) globals().whatever

namespace rubinius {
  VM::VM(uint32_t id, SharedState& shared, const char* name)
    : memory::ManagedThread(id, shared, memory::ManagedThread::eThread, name)
    , call_frame_(NULL)
    , thread_nexus_(shared.thread_nexus())
    , park_(new Park)
    , stack_start_(0)
    , stack_size_(0)
    , stack_cushion_(shared.config.machine_stack_cushion.value)
    , interrupt_with_signal_(false)
    , interrupt_by_kill_(false)
    , check_local_interrupts_(false)
    , thread_step_(false)
    , fiber_wait_mutex_()
    , fiber_wait_condition_()
    , fiber_transition_flag_(eSuspending)
    , interrupt_lock_()
    , method_missing_reason_(eNone)
    , constant_missing_reason_(vFound)
    , zombie_(false)
    , main_thread_(false)
    , thread_phase_(ThreadNexus::eUnmanaged)
    , sample_interval_(0)
    , sample_counter_(0)
    , shared(shared)
    , waiting_channel_(this, nil<Channel>())
    , interrupted_exception_(this, nil<Exception>())
    , thread_(this, nil<Thread>())
    , fiber_(this, nil<Fiber>())
    , waiting_object_(this, cNil)
    , start_time_(0)
    , native_method_environment(NULL)
    , custom_wakeup_(NULL)
    , custom_wakeup_data_(NULL)
    , thread_state_(this)
  {
    if(memory()) {
      local_slab_.refill(0, 0);
    }

    set_sample_interval();
  }

  VM::~VM() {
    if(park_) {
      delete park_;
      park_ = NULL;
    }
  }

  void VM::discard(STATE, VM* vm) {
    state->vm()->metrics()->threads_destroyed++;

    delete vm;
  }

  void VM::set_thread(Thread* thread) {
    thread_.set(thread);
  }

  void VM::set_fiber(Fiber* fiber) {
    fiber_.set(fiber);
  }

  void VM::set_start_time() {
    start_time_ = get_current_time();
  }

  double VM::run_time() {
    return timer::time_elapsed_seconds(start_time_);
  }

  void VM::checkpoint(STATE) {
    metrics()->checkpoints++;

    thread_nexus_->check_stop(state, this, [this, state]{
        metrics()->stops++;
        collect_maybe(state);
      });

    if(sample_counter_++ >= sample_interval_) {
      sample(state);
      set_sample_interval();
    }
  }

  void VM::raise_stack_error(STATE) {
    state->raise_stack_error(state);
  }

  void VM::validate_stack_size(STATE, size_t size) {
    if(stack_cushion_ > size) {
      Exception::raise_runtime_error(state, "requested stack size is invalid");
    }
  }

  bool VM::push_call_frame(STATE, CallFrame* frame, CallFrame*& previous_frame) {
    if(!check_stack(state, frame)) return false;

    previous_frame = call_frame_;
    frame->previous = call_frame_;
    call_frame_ = frame;

    return true;
  }

  bool VM::check_thread_raise_or_kill(STATE) {
    Exception* exc = interrupted_exception();

    if(!exc->nil_p()) {
      clear_interrupted_exception();

      // Only write the locations if there are none.
      if(exc->locations()->nil_p() || exc->locations()->size() == 0) {
        exc->locations(this, Location::from_call_stack(state));
      }

      thread_state_.raise_exception(exc);

      return true;
    }

    if(interrupt_by_kill()) {
      if(state->vm()->thread()->current_fiber()->root_p()) {
        clear_interrupt_by_kill();
      } else {
        set_check_local_interrupts();
      }

      thread_state_.raise_thread_kill();

      return true;
    }

    // If the current thread is trying to step, debugger wise, then assist!
    if(thread_step()) {
      clear_thread_step();
      if(!Helpers::yield_debugger(state, cNil)) return true;
    }

    return false;
  }

  CallFrame* VM::get_call_frame(ssize_t up) {
    CallFrame* frame = call_frame_;

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    return frame;
  }

  CallFrame* VM::get_ruby_frame(ssize_t up) {
    CallFrame* frame = call_frame_;

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    while(frame) {
      if(!frame->native_method_p()) return frame;
      frame = frame->previous;
    }

    return NULL;
  }

  CallFrame* VM::get_noncore_frame(STATE) {
    for(CallFrame* frame = call_frame_; frame; frame = frame->previous) {
      if(frame->native_method_p()) continue;

      CompiledCode* code = frame->compiled_code;
      if(code && !code->nil_p()) {
        if(!code->core_method(state)) return frame;
      }
    }

    return NULL;
  }

  CallFrame* VM::get_filtered_frame(STATE, const std::regex& filter) {
    for(CallFrame* frame = call_frame_; frame; frame = frame->previous) {
      if(frame->native_method_p()) continue;

      CompiledCode* code = frame->compiled_code;
      if(code && !code->nil_p() && !code->file()->nil_p()) {
        if(!std::regex_match(code->file()->cpp_str(state), filter)) {
          return frame;
        }
      }
    }

    return NULL;
  }

  CallFrame* VM::get_variables_frame(ssize_t up) {
    CallFrame* frame = call_frame_;

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    while(frame) {
      if(!frame->is_inline_block()
          && !frame->native_method_p()
          && frame->scope)
      {
        return frame;
      }

      frame = frame->previous;
    }

    return NULL;
  }

  CallFrame* VM::get_scope_frame(ssize_t up) {
    CallFrame* frame = call_frame_;

    while(frame && up-- > 0) {
      frame = frame->previous;
    }

    while(frame) {
      if(frame->scope) return frame;
      frame = frame->previous;
    }

    return NULL;
  }

  bool VM::scope_valid_p(VariableScope* scope) {
    CallFrame* frame = call_frame_;

    while(frame) {
      if(frame->scope && frame->scope->on_heap() == scope) {
        return true;
      }

      frame = frame->previous;
    }

    return false;
  }

  void VM::collect_maybe(STATE) {
    memory()->collect_maybe(state);
  }

  void VM::sample(STATE) {
    timer::StopWatch<timer::nanoseconds> timer(metrics()->sample_ns);

    metrics()->samples++;

    CallFrame* frame = state->vm()->call_frame();

    while(frame) {
      // TODO: add counters to native method frames
      if(frame->compiled_code) {
        frame->compiled_code->machine_code()->sample_count++;
      }

      frame = frame->previous;
    }
  }

  static void suspend_thread() {
    static int i = 0;
    static int delay[] = {
      45, 17, 38, 31, 10, 40, 13, 37, 16, 37, 1, 20, 23, 43, 38, 4, 2, 26, 25, 5
    };
    static int modulo = sizeof(delay) / sizeof(int);
    static struct timespec ts = {0, 0};

    ts.tv_nsec = delay[i++ % modulo];

    nanosleep(&ts, NULL);
  }

  void VM::set_zombie(STATE) {
    state->shared().thread_nexus()->delete_vm(this);
    set_zombie();
  }

  void VM::set_zombie() {
    set_thread(nil<Thread>());
    set_fiber(nil<Fiber>());
    zombie_ = true;
  }

  /**
   * Returns the current VM executing on this pthread.
   */
  VM* VM::current() {
    return memory::ManagedThread::current()->as_vm();
  }

  /**
   * Sets this VM instance as the current VM on this pthread.
   */
  void VM::set_current_thread() {
    memory::ManagedThread::set_current_thread(this);
  }

  void type_assert(STATE, Object* obj, object_type type, const char* reason) {
    if((obj->reference_p() && obj->type_id() != type)
        || (type == FixnumType && !obj->fixnum_p())) {
      std::ostringstream msg;
      msg << reason << ": " << obj->to_string(state, true);
      Exception::raise_type_error(state, type, obj, msg.str().c_str());
    }
  }

  TypeInfo* VM::find_type(int type) {
    return memory()->type_info[type];
  }

  void VM::after_fork_child(STATE) {
    thread_nexus_->after_fork_child(state);

    interrupt_lock_.unlock();
    set_main_thread();

    // TODO: Remove need for root_vm.
    state->shared().env()->set_root_vm(state->vm());

    state->shared().env()->after_fork_child(state);
  }

  void VM::set_const(const char* name, Object* val) {
    State state(this);
    globals().object->set_const(&state, (char*)name, val);
  }

  void VM::set_const(Module* mod, const char* name, Object* val) {
    State state(this);
    mod->set_const(&state, (char*)name, val);
  }

  Object* VM::path2class(const char* path) {
    State state(this);
    Module* mod = shared.globals.object.get();

    char* copy = strdup(path);
    char* cur = copy;

    for(;;) {
      char* pos = strstr(cur, "::");
      if(pos) *pos = 0;

      Object* obj = mod->get_const(&state, state.symbol(cur));

      if(pos) {
        if(Module* m = try_as<Module>(obj)) {
          mod = m;
        } else {
          free(copy);
          return cNil;
        }
      } else {
        free(copy);
        return obj;
      }

      cur = pos + 2;
    }
  }

  void VM::print_backtrace() {
    abort();
  }

  bool VM::wakeup(STATE) {
    std::lock_guard<locks::spinlock_mutex> guard(interrupt_lock_);

    set_check_local_interrupts();
    Object* wait = waiting_object_.get();

    if(park_->parked_p()) {
      park_->unpark();
      return true;
    } else if(interrupt_with_signal_) {
#ifdef RBX_WINDOWS
      // TODO: wake up the thread
#else
      pthread_kill(os_thread_, SIGVTALRM);
#endif
      interrupt_lock_.unlock();
      // Wakeup any locks hanging around with contention
      // TODO: MemoryHeader
      // memory()->release_contention(state);
      return true;
    } else if(!wait->nil_p()) {
      // We shouldn't hold the VM lock and the IH lock at the same time,
      // other threads can grab them and deadlock.
      // TODO: MemoryHeader
      // InflatedHeader* ih = wait->inflated_header(state);
      // interrupt_lock_.unlock();
      // ih->wakeup(state, wait);
      return true;
    } else {
      Channel* chan = waiting_channel_.get();

      if(!chan->nil_p()) {
        interrupt_lock_.unlock();
        // TODO: MemoryHeader
        // memory()->release_contention(state);
        chan->send(state, cNil);
        return true;
      } else if(custom_wakeup_) {
        interrupt_lock_.unlock();
        // TODO: MemoryHeader
        // memory()->release_contention(state);
        (*custom_wakeup_)(custom_wakeup_data_);
        return true;
      }

      return false;
    }
  }

  void VM::clear_waiter() {
    utilities::thread::SpinLock::LockGuard guard(shared.wait_lock());

    interrupt_with_signal_ = false;
    waiting_channel_.set(nil<Channel>());
    waiting_object_.set(cNil);
    custom_wakeup_ = 0;
    custom_wakeup_data_ = 0;
  }

  void VM::wait_on_channel(Channel* chan) {
    std::lock_guard<locks::spinlock_mutex> guard(interrupt_lock_);

    thread()->sleep(this, cTrue);
    waiting_channel_.set(chan);
  }

  void VM::wait_on_inflated_lock(Object* wait) {
    utilities::thread::SpinLock::LockGuard guard(shared.wait_lock());

    waiting_object_.set(wait);
  }

  void VM::wait_on_custom_function(void (*func)(void*), void* data) {
    utilities::thread::SpinLock::LockGuard guard(shared.wait_lock());

    custom_wakeup_ = func;
    custom_wakeup_data_ = data;
  }

  void VM::set_sleeping() {
    thread()->sleep(this, cTrue);
  }

  void VM::clear_sleeping() {
    thread()->sleep(this, cFalse);
  }

  void VM::reset_parked() {
    park_->reset_parked();
  }

  void VM::register_raise(STATE, Exception* exc) {
    std::lock_guard<locks::spinlock_mutex> guard(interrupt_lock_);
    interrupted_exception_.set(exc);
    set_check_local_interrupts();
  }

  void VM::register_kill(STATE) {
    std::lock_guard<locks::spinlock_mutex> guard(interrupt_lock_);
    set_interrupt_by_kill();
    set_check_local_interrupts();
  }

  memory::VariableRootBuffers& VM::current_root_buffers() {
    return variable_root_buffers();
  }

  void VM::gc_scan(memory::GarbageCollector* gc) {
    gc->walk_call_frame(call_frame_);
  }

  void VM::gc_verify(memory::GarbageCollector* gc) {
    gc->verify_call_frame(call_frame_);
  }
};
