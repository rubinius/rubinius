#include "vm.hpp"
#include "state.hpp"
#include "memory.hpp"
#include "global_cache.hpp"
#include "environment.hpp"
#include "thread_phase.hpp"

#include "memory/gc.hpp"

#include "object_utils.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/array.hpp"
#include "builtin/list.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/system.hpp"
#include "builtin/fiber.hpp"
#include "builtin/location.hpp"
#include "builtin/native_method.hpp"
#include "builtin/channel.hpp"
#include "builtin/call_site.hpp"
#include "builtin/exception.hpp"
#include "builtin/jit.hpp"
#include "variable_scope.hpp"

#include "instruments/timing.hpp"

#include "config_parser.hpp"
#include "config.h"

#include "call_frame.hpp"
#include "signal.hpp"
#include "configuration.hpp"
#include "helpers.hpp"

#include "util/thread.hpp"

#include "park.hpp"

#include <iostream>
#include <iomanip>
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
    : memory::ManagedThread(id, shared, memory::ManagedThread::eRuby, name)
    , call_frame_(NULL)
    , thread_nexus_(shared.thread_nexus())
    , saved_call_site_information_(NULL)
    , fiber_stacks_(this, shared)
    , park_(new Park)
    , stack_start_(0)
    , stack_size_(0)
    , current_stack_start_(0)
    , current_stack_size_(0)
    , interrupt_lock_()
    , method_missing_reason_(eNone)
    , constant_missing_reason_(vFound)
    , zombie_(false)
    , main_thread_(false)
    , thread_phase_(ThreadNexus::cUnmanaged)
    , shared(shared)
    , waiting_channel_(this, nil<Channel>())
    , interrupted_exception_(this, nil<Exception>())
    , thread(this, nil<Thread>())
    , current_fiber(this, nil<Fiber>())
    , root_fiber(this, nil<Fiber>())
    , waiting_object_(this, cNil)
    , native_method_environment(NULL)
    , custom_wakeup_(NULL)
    , custom_wakeup_data_(NULL)
    , thread_state_(this)
  {
    if(memory()) {
      local_slab_.refill(0, 0);
    }

    allocation_tracking_ = shared.config.allocation_tracking;
  }

  VM::~VM() {
    delete park_;
  }

  void VM::discard(STATE, VM* vm) {
    state->vm()->metrics().system.threads_destroyed++;

    delete vm;
  }

  void VM::set_stack_bounds(size_t size) {
    void* stack_address;

    stack_size_ = size;
    stack_start_ = &stack_address;

    set_stack_bounds(stack_start_, stack_size_);
  }

  void VM::raise_stack_error(STATE) {
    state->raise_stack_error(state);
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
      Fiber* fib = current_fiber.get();

      if(fib->nil_p() || fib->root_p()) {
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

  void VM::blocking_suspend(STATE, metrics::metric& counter) {
    timer::StopWatch<timer::milliseconds> timer(counter);

    BlockPhase blocking(state);
    suspend_thread();
  }

  void VM::sleeping_suspend(STATE, metrics::metric& counter) {
    timer::StopWatch<timer::milliseconds> timer(counter);

    SleepPhase sleeping(state);
    suspend_thread();
  }

  void VM::become_managed() {
    thread_nexus_->managed_lock(this);
  }

  void VM::set_zombie(STATE) {
    state->shared().thread_nexus()->delete_vm(this);
    thread.set(nil<Thread>());
    zombie_ = true;
  }

  void VM::initialize_config() {
    State state(this);

#ifdef ENABLE_LLVM
    Array* ary = Array::create(&state, 3);

    G(jit)->available(&state, cTrue);
    G(jit)->properties(&state, ary);

    if(!shared.config.jit_disabled) {
      ary->append(&state, state.symbol("usage"));
      if(shared.config.jit_inline_generic) {
        ary->append(&state, state.symbol("inline_generic"));
      }

      if(shared.config.jit_inline_blocks) {
        ary->append(&state, state.symbol("inline_blocks"));
      }
      G(jit)->enabled(&state, cTrue);
    } else {
      G(jit)->enabled(&state, cFalse);
    }
#else
    G(jit)->available(&state, cFalse);
    G(jit)->properties(&state, nil<Array>());
    G(jit)->enabled(&state, cFalse);
#endif
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

    interrupt_lock_.init();
    set_main_thread();
    become_managed();

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

  void VM::interrupt_with_signal() {
    vm_jit_.interrupt_with_signal_ = true;
  }

  bool VM::wakeup(STATE) {
    utilities::thread::SpinLock::LockGuard guard(interrupt_lock_);

    set_check_local_interrupts();
    Object* wait = waiting_object_.get();

    if(park_->parked_p()) {
      park_->unpark();
      return true;
    } else if(vm_jit_.interrupt_with_signal_) {
#ifdef RBX_WINDOWS
      // TODO: wake up the thread
#else
      pthread_kill(os_thread_, SIGVTALRM);
#endif
      interrupt_lock_.unlock();
      // Wakeup any locks hanging around with contention
      memory()->release_contention(state);
      return true;
    } else if(!wait->nil_p()) {
      // We shouldn't hold the VM lock and the IH lock at the same time,
      // other threads can grab them and deadlock.
      InflatedHeader* ih = wait->inflated_header(state);
      interrupt_lock_.unlock();
      ih->wakeup(state, wait);
      return true;
    } else {
      Channel* chan = waiting_channel_.get();

      if(!chan->nil_p()) {
        interrupt_lock_.unlock();
        memory()->release_contention(state);
        chan->send(state, cNil);
        return true;
      } else if(custom_wakeup_) {
        interrupt_lock_.unlock();
        memory()->release_contention(state);
        (*custom_wakeup_)(custom_wakeup_data_);
        return true;
      }

      return false;
    }
  }

  void VM::clear_waiter() {
    utilities::thread::SpinLock::LockGuard guard(shared.wait_lock());

    vm_jit_.interrupt_with_signal_ = false;
    waiting_channel_.set(nil<Channel>());
    waiting_object_.set(cNil);
    custom_wakeup_ = 0;
    custom_wakeup_data_ = 0;
  }

  void VM::wait_on_channel(Channel* chan) {
    utilities::thread::SpinLock::LockGuard guard(interrupt_lock_);

    thread->sleep(this, cTrue);
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
    thread->sleep(this, cTrue);
  }

  void VM::clear_sleeping() {
    thread->sleep(this, cFalse);
  }

  void VM::reset_parked() {
    park_->reset_parked();
  }

  void VM::register_raise(STATE, Exception* exc) {
    utilities::thread::SpinLock::LockGuard guard(interrupt_lock_);
    interrupted_exception_.set(exc);
    set_check_local_interrupts();
  }

  void VM::register_kill(STATE) {
    utilities::thread::SpinLock::LockGuard guard(interrupt_lock_);
    set_interrupt_by_kill();
    set_check_local_interrupts();
  }

  void VM::set_current_fiber(Fiber* fib) {
    if(fib->root_p()) {
      restore_stack_bounds();
    } else {
      set_stack_bounds(fib->stack_start(), fib->stack_size());
    }

    current_fiber.set(fib);
  }

  memory::VariableRootBuffers& VM::current_root_buffers() {
    if(current_fiber->nil_p() || current_fiber->root_p()) {
      return variable_root_buffers();
    }

    return current_fiber->variable_root_buffers();
  }

  void VM::gc_scan(memory::GarbageCollector* gc) {
    gc->walk_call_frame(call_frame_);
  }

  void VM::gc_fiber_clear_mark() {
    fiber_stacks_.gc_clear_mark();
  }

  void VM::gc_fiber_scan(memory::GarbageCollector* gc, bool only_marked) {
    fiber_stacks_.gc_scan(gc, only_marked);
  }

  void VM::gc_verify(memory::GarbageCollector* gc) {
    gc->verify_call_frame(call_frame_);
  }
};
