#include "arguments.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"
#include "memory.hpp"
#include "metrics.hpp"
#include "raise_reason.hpp"
#include "thread_phase.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/fiber.hpp"
#include "class/lookup_table.hpp"
#include "class/native_method.hpp"
#include "class/object.hpp"
#include "class/thread.hpp"

#include "memory/gc.hpp"

#include "dtrace/dtrace.h"

#include "logger.hpp"

#include "missing/gettid.h"

#include <ostream>
#include <regex>
#include <string>

namespace rubinius {
  std::atomic<uint32_t> Fiber::fiber_ids_;

  void Fiber::bootstrap(STATE) {
    GO(fiber).set(state->memory()->new_class<Class, Fiber>(state, "Fiber"));

    fiber_ids_.store(0);
  }

  bool Fiber::root_p() {
    return vm()->fiber()->vm() == vm()->thread()->vm();
  }

  void Fiber::unpack_arguments(STATE, Arguments& args) {
    switch(args.total()) {
      case 0:
        state->vm()->thread()->fiber_value(state, cNil);
        break;
      case 1:
        state->vm()->thread()->fiber_value(state, args.get_argument(0));
        break;
      default:
        state->vm()->thread()->fiber_value(state,  args.as_array(state));
        break;
    }
  }

  void Fiber::start(STATE, Arguments& args) {
    state->vm()->thread()->fiber_value(state, args.as_array(state));

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, stack_size()->to_native());
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    int status = pthread_create(&vm()->os_thread(), &attrs,
        Fiber::run, (void*)vm());

    pthread_attr_destroy(&attrs);

    if(status != 0) {
      char buf[RBX_STRERROR_BUFSIZE];
      char* err = RBX_STRERROR(status, buf, RBX_STRERROR_BUFSIZE);
      Exception::raise_fiber_error(state, err);
    }

    // Wait for Fiber thread to start up and pause.
    while(!vm()->suspended_p()) {
      ; // spin wait
    }
  }

  void Fiber::restart(STATE) {
    UnmanagedPhase unmanaged(state);

    {
      std::lock_guard<std::mutex> guard(state->vm()->thread()->fiber_mutex());

      if(!vm()->suspended_p()) {
        std::ostringstream msg;
        msg << "attempt to restart non-suspended ("
          << vm()->fiber_transition_flag() << ") fiber";
        Exception::raise_fiber_error(state, msg.str().c_str());
      }

      state->vm()->set_suspending();

      restart_context(state->vm());
      wakeup();

      while(vm()->suspended_p()) {
        std::lock_guard<std::mutex> guard(vm()->fiber_wait_mutex());
        vm()->fiber_wait_condition().notify_one();
      }
    }

    while(!vm()->running_p()) {
      ; // spin wait
    }
  }

  void Fiber::cancel(STATE) {
    {
      std::lock_guard<std::mutex> guard(state->vm()->thread()->fiber_mutex());

      vm()->thread_state()->raise_fiber_cancel();

      state->vm()->set_suspending();

      restart_context(state->vm());
      wakeup();

      while(vm()->suspended_p()) {
        std::lock_guard<std::mutex> guard(vm()->fiber_wait_mutex());
        vm()->fiber_wait_condition().notify_one();
      }
    }

    vm()->limited_wait_for([this]{ return vm()->running_p(); });

    // Release the canceled Fiber.
    state->vm()->set_suspended();

    vm()->limited_wait_for([this]{ return vm()->zombie_p(); });

    vm()->set_canceled();

    state->vm()->set_running();
  }

  void Fiber::suspend_and_continue(STATE) {
    UnmanagedPhase unmanaged(state);

    {
      std::unique_lock<std::mutex> lk(vm()->fiber_wait_mutex());

      vm()->set_suspended();
      while(!wakeup_p()) {
        vm()->fiber_wait_condition().wait(lk);
      }
      clear_wakeup();
      vm()->set_resuming();
    }

    {
      std::lock_guard<std::mutex> guard(state->vm()->thread()->fiber_mutex());

      vm()->set_running();

      while(!restart_context()->suspended_p()) {
        ; // spin wait
      }

      state->vm()->thread()->current_fiber(state, this);
    }
  }

  Object* Fiber::return_value(STATE) {
    if(vm()->thread_state()->raise_reason() == cNone) {
      return state->vm()->thread()->fiber_value();
    } else if(vm()->thread_state()->raise_reason() == cFiberCancel) {
      return NULL;
    } else {
      invoke_context()->thread_state()->set_state(vm()->thread_state());
      return NULL;
    }
  }

  void* Fiber::run(void* ptr) {
    VM* vm = reinterpret_cast<VM*>(ptr);
    State state_obj(vm), *state = &state_obj;

    vm->set_stack_bounds(vm->fiber()->stack_size()->to_native());
    vm->set_current_thread();
    vm->set_start_time();

    RUBINIUS_THREAD_START(
        const_cast<RBX_DTRACE_CHAR_P>(
          vm->name().c_str()), vm->fiber()->fiber_id()->to_native(), 0);

    vm->fiber()->pid(state, Fixnum::from(gettid()));

    if(state->shared().config.machine_thread_log_lifetime.value) {
      logger::write("fiber: run: %s, %d, %#x",
          vm->name().c_str(), vm->fiber()->pid()->to_native(),
          (intptr_t)pthread_self());
    }

    NativeMethod::init_thread(state);

    vm->fiber()->suspend_and_continue(state);

    Object* value = vm->fiber()->block()->send(state, G(sym_call),
        as<Array>(vm->thread()->fiber_value()), vm->fiber()->block());
    vm->set_call_frame(NULL);

    if(value) {
      vm->thread()->fiber_value(state, value);
    } else {
      vm->thread()->fiber_value(state, cNil);
    }

    if(vm->thread_state()->raise_reason() != cFiberCancel) {
      if(vm->fiber()->status() == eTransfer) {
        // restart the root Fiber
        vm->thread()->fiber()->invoke_context(vm);
        vm->thread()->fiber()->restart(state);
      } else {
        vm->fiber()->invoke_context()->fiber()->restart(state);
      }
    }

    {
      std::lock_guard<std::mutex> guard(vm->fiber_wait_mutex());

      vm->fiber()->status(eDead);
      vm->set_suspended();
    }

    vm->unmanaged_phase(state);

    state->shared().report_profile(state);

    NativeMethod::cleanup_thread(state);

    if(state->shared().config.machine_fiber_log_lifetime.value) {
      logger::write("fiber: exit: %s %fs", vm->name().c_str(), vm->run_time());
    }

    vm->set_zombie(state);

    RUBINIUS_THREAD_STOP(
        const_cast<RBX_DTRACE_CHAR_P>(
          vm->name().c_str()), vm->fiber()->fiber_id()->to_native(), 0);

    return 0;
  }

  Fiber* Fiber::current(STATE) {
    return state->vm()->fiber();
  }

  /* This creates the pseudo-Fiber for the Thread. This provides a uniform
   * means of expressing things like Fiber.current.
   */
  Fiber* Fiber::create(STATE, VM* vm) {
    Fiber* fiber = state->memory()->new_object_pinned<Fiber>(state, G(fiber));

    vm->set_fiber(fiber);
    vm->set_running();

    fiber->vm(vm);
    fiber->pid(vm->thread()->pid());
    fiber->stack_size(vm->thread()->stack_size());
    fiber->thread_name(state, String::create(state, vm->name().c_str()));
    fiber->fiber_id(Fixnum::from(0));
    fiber->status(eRunning);
    fiber->invoke_context(vm);

    return fiber;
  }

  Fiber* Fiber::create(STATE, Object* self, Object* stack_size, Object* block) {
    Fiber* fiber = state->memory()->new_object_pinned<Fiber>(state, as<Class>(self));
    fiber->block(state, block);

    std::ostringstream name;
    name << "fiber." << fiber->fiber_id()->to_native();

    {
      BlockPhase blocking(state);
      fiber->vm(state->vm()->thread_nexus()->new_vm(&state->shared(), name.str().c_str()));
    }

    fiber->vm()->set_kind(memory::ManagedThread::eFiber);
    fiber->vm()->set_suspending();

    if(Fixnum* size = try_as<Fixnum>(stack_size)) {
      fiber->vm()->validate_stack_size(state, size->to_native());
      fiber->stack_size(state, size);
    }

    fiber->vm()->set_thread(state->vm()->thread());
    fiber->vm()->set_fiber(fiber);

    state->memory()->native_finalizer(state, fiber,
        (memory::FinalizerFunction)&Fiber::finalize);

    if(state->shared().config.machine_fiber_log_lifetime.value) {
      const std::regex& filter = state->shared().config.machine_fiber_log_filter();

      if(CallFrame* call_frame = state->vm()->get_filtered_frame(state, filter)) {
        std::ostringstream source;

        source << call_frame->file(state)->cpp_str(state).c_str()
          << ":" << call_frame->line(state);

        logger::write("fiber: new: %s, %d, %s",
            fiber->thread_name()->c_str(state),
            fiber->fiber_id()->to_native(), source.str().c_str());

        fiber->source(state, String::create(state, source.str().c_str()));
      } else {
        logger::write("fiber: new: %s, %d",
            fiber->thread_name()->c_str(state), fiber->fiber_id()->to_native());
      }
    }

    state->vm()->metrics().system.fibers_created++;

    return fiber;
  }

  String* Fiber::status(STATE) {
    switch(status()) {
      case eCreated:
        return String::create(state, "created");
        break;
      case eRunning:
        return String::create(state, "run");
        break;
      case eYielding:
        return String::create(state, "yield");
        break;
      case eTransfer:
        return String::create(state, "transfer");
        break;
      case eDead:
        return String::create(state, "dead");
        break;
    }

    return String::create(state, "unknown");
  }

  Object* Fiber::resume(STATE, Arguments& args) {
    {
      std::lock_guard<std::mutex> guard(state->vm()->thread()->fiber_mutex());

      if(state->vm()->thread() != thread()) {
        Exception::raise_fiber_error(state, "attempt to resume fiber across threads");
      } else if(status() == eTransfer) {
        Exception::raise_fiber_error(state, "attempt to resume transfered fiber");
      } else if(status() == eRunning) {
        Exception::raise_fiber_error(state, "attempt to resume running fiber");
      } else if(status() == eDead) {
        Exception::raise_fiber_error(state, "attempt to resume dead fiber");
      } else if(root_p()) {
        Exception::raise_fiber_error(state, "attempt to resume root fiber");
      }

      unpack_arguments(state, args);
      invoke_context(state->vm());

      if(status() == eCreated) {
        start(state, args);
      }

      status(eRunning);
    }

    // Being cooperative...
    restart(state);

    // Through the worm hole...
    state->vm()->fiber()->suspend_and_continue(state);

    // We're back...
    return return_value(state);
  }

  Object* Fiber::transfer(STATE, Arguments& args) {
    {
      std::lock_guard<std::mutex> guard(state->vm()->thread()->fiber_mutex());

      if(state->vm()->thread() != thread()) {
        Exception::raise_fiber_error(state, "attempt to transfer fiber across threads");
      } else if(status() == eDead) {
        Exception::raise_fiber_error(state, "attempt to transfer to dead fiber");
      } else if(state->vm()->fiber() == this) {
        // This should arguably be a FiberError
        return args.as_array(state);
      }

      unpack_arguments(state, args);
      invoke_context(state->vm());

      if(status() == eCreated) {
        start(state, args);
      }

      status(eTransfer);
    }

    // Being cooperative...
    restart(state);

    // Through the worm hole...
    state->vm()->fiber()->suspend_and_continue(state);

    // We're back...
    return return_value(state);
  }

  Object* Fiber::dispose(STATE) {
    cancel(state);

    return this;
  }

  Object* Fiber::s_yield(STATE, Arguments& args) {
    Fiber* fiber = state->vm()->fiber();
    OnStack<1> os(state, fiber);

    {
      std::lock_guard<std::mutex> guard(state->vm()->thread()->fiber_mutex());

      if(fiber->root_p()) {
        Exception::raise_fiber_error(state, "can't yield from root fiber");
      } else if(fiber->status() == eTransfer) {
        Exception::raise_fiber_error(state, "can't yield from transferred fiber");
      }

      fiber->unpack_arguments(state, args);
      fiber->status(eYielding);
    }

    // Being cooperative...
    fiber->invoke_context()->fiber()->restart(state);

    // Through the worm hole...
    fiber->suspend_and_continue(state);

    // We're back...
    return fiber->return_value(state);
  }

  Array* Fiber::s_list(STATE) {
    return state->shared().vm_fibers(state);
  }

  Fiber* Fiber::s_main(STATE) {
    return state->vm()->thread()->fiber();
  }

  Fixnum* Fiber::s_count(STATE) {
    return state->shared().vm_fibers_count(state);
  }

  void Fiber::finalize(STATE, Fiber* fiber) {
    if(state->shared().config.machine_fiber_log_finalizer.value) {
      logger::write("fiber: finalizer: %s, %d",
          fiber->thread_name()->c_str(state), fiber->fiber_id()->to_native());
    }

    if(fiber->vm()) {
      if(!state->shared().halting_p()) {
        if(!fiber->vm()->zombie_p()) {
          fiber->cancel(state);
        }
      }

      if(fiber->vm()->zombie_p()) {
        VM::discard(state, fiber->vm());
        fiber->vm(NULL);
      } else {
        logger::write("fiber: finalizer: fiber not completed: %s, %d",
            fiber->thread_name()->c_str(state), fiber->fiber_id()->to_native());
      }
    }
  }
}
