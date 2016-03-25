#include "vm.hpp"
#include "state.hpp"
#include "memory.hpp"
#include "global_cache.hpp"
#include "environment.hpp"
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

#include "instruments/tooling.hpp"
#include "instruments/rbxti-internal.hpp"
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

  unsigned long VM::cStackDepthMax = 655300;

#ifndef RBX_WINDOWS
  /**
   * Maximum amount of stack space to use.
   * getrlimit can report there is 4G of stack (ie, unlimited).  Even when
   * there is unlimited stack, we clamp the max to this value (currently 128M).
   */
  static rlim_t cMaxStack = (1024 * 1024 * 128);
#endif

  VM::VM(uint32_t id, SharedState& shared, const char* name)
    : memory::ManagedThread(id, shared, memory::ManagedThread::eRuby, name)
    , call_frame_(NULL)
    , thread_nexus_(shared.thread_nexus())
    , saved_call_site_information_(NULL)
    , fiber_stacks_(this, shared)
    , park_(new Park)
    , tooling_env_(NULL)
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

    tooling_env_ = rbxti::create_env(this);
    tooling_ = false;

    allocation_tracking_ = shared.config.allocation_tracking;
  }

  VM::~VM() {
    rbxti::destroy_env(tooling_env_);
    delete park_;
  }

  void VM::discard(STATE, VM* vm) {
    state->vm()->metrics().system.threads_destroyed++;

    delete vm;
  }

  void VM::push_call_frame(CallFrame* frame, CallFrame*& previous_frame) {
    previous_frame = call_frame_;
    frame->previous = call_frame_;
    call_frame_ = frame;
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

  void VM::init_stack_size() {
#ifndef RBX_WINDOWS
    struct rlimit rlim;
    if(getrlimit(RLIMIT_STACK, &rlim) == 0) {
      rlim_t space = rlim.rlim_cur/5;

      if(space > 1024*1024) space = 1024*1024;
      rlim_t adjusted = (rlim.rlim_cur - space);

      if(adjusted > cMaxStack) {
        cStackDepthMax = cMaxStack;
      } else {
        cStackDepthMax = adjusted;
      }
    }
#endif
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
    set_stack_bounds((uintptr_t)fib->stack_start(), fib->stack_size());
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

    if(CallSiteInformation* info = saved_call_site_information()) {
      info->executable = as<Executable>(gc->mark_object(info->executable));
    }

    State ls(this);

    shared.tool_broker()->at_gc(&ls);
  }

  void VM::gc_fiber_clear_mark() {
    fiber_stacks_.gc_clear_mark();
  }

  void VM::gc_fiber_scan(memory::GarbageCollector* gc, bool only_marked) {
    fiber_stacks_.gc_scan(gc, only_marked);
  }

  void VM::gc_verify(memory::GarbageCollector* gc) {
    gc->verify_call_frame(call_frame_);

    if(CallSiteInformation* info = saved_call_site_information()) {
      info->executable->validate();
    }
  }
};
