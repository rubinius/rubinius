#include "arguments.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"
#include "memory.hpp"
#include "metrics.hpp"
#include "raise_reason.hpp"
#include "thread_phase.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fiber.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/native_method.hpp"
#include "builtin/object.hpp"
#include "builtin/thread.hpp"

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
        value(state, cNil);
        break;
      case 1:
        value(state, args.get_argument(0));
        break;
      default:
        value(state,  args.as_array(state));
        break;
    }
  }

  void Fiber::start(STATE, Arguments& args) {
    arguments(state, args.as_array(state));

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, stack_size()->to_native());
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    pthread_create(&vm()->os_thread(), &attrs,
        Fiber::run, (void*)vm());

    pthread_attr_destroy(&attrs);

    // Wait for Fiber thread to start up and pause.
    while(!vm()->wait_flag());
  }

  void Fiber::restart(STATE) {
    std::lock_guard<std::mutex> guard(state->vm()->thread()->fiber_mutex());

    state->vm()->thread()->current_fiber(state, this);

    while(vm()->wait_flag()) {
      std::lock_guard<std::mutex> guard(vm()->wait_mutex());
      vm()->wait_condition().notify_one();
    }
  }

  void Fiber::suspend(STATE) {
    {
      std::unique_lock<std::mutex> lk(vm()->wait_mutex());
      vm()->set_wait_flag(true);

      {
        UnmanagedPhase unmanaged(state);
        vm()->wait_condition().wait(lk);
      }

      vm()->set_wait_flag(false);
    }

    std::lock_guard<std::mutex> guard(state->vm()->thread()->fiber_mutex());
  }

  Object* Fiber::return_value(STATE) {
    if(vm()->thread_state()->raise_reason() == cNone) {
      return value();
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

    vm->fiber()->suspend(state);

    Object* value = vm->fiber()->block()->send(state, G(sym_call),
        vm->fiber()->arguments(), vm->fiber()->block());
    vm->set_call_frame(NULL);

    if(value) {
      vm->fiber()->value(state, value);
    } else {
      vm->fiber()->value(state, cNil);
    }

    vm->fiber()->invoke_context()->fiber()->restart(state);

    vm->fiber()->status(eDead);
    vm->fiber()->vm()->set_wait_flag(true);

    vm->unmanaged_phase();

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
    Fiber* fiber = state->memory()->new_object<Fiber>(state, G(fiber));

    vm->set_fiber(fiber);
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
    Fiber* fiber = state->memory()->new_object<Fiber>(state, as<Class>(self));
    fiber->block(state, block);

    std::ostringstream name;
    name << "fiber." << fiber->fiber_id()->to_native();

    fiber->vm(state->vm()->thread_nexus()->new_vm(&state->shared(), name.str().c_str()));

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
    if(status() == eCreated) {
      start(state, args);
    } else if(status() == eTransfer) {
      Exception::raise_fiber_error(state, "attempt to resume transfered fiber");
    } else if(status() == eRunning) {
      Exception::raise_fiber_error(state, "attempt to resume running fiber");
    } else if(status() == eDead) {
      Exception::raise_fiber_error(state, "attempt to resume dead fiber");
    } else if(root_p()) {
      Exception::raise_fiber_error(state, "attempt to resume root fiber");
    }

    status(eRunning);
    unpack_arguments(state, args);
    invoke_context(state->vm());

    // Being cooperative...
    restart(state);

    // Through the worm hole...
    state->vm()->fiber()->suspend(state);

    // We're back...
    return return_value(state);
  }

  Object* Fiber::transfer(STATE, Arguments& args) {
    if(status() == eCreated) {
      start(state, args);
    } else if(state->vm()->fiber() == this) {
      return args.as_array(state);
    } else if(status() == eDead) {
      Exception::raise_fiber_error(state, "attempt to transfer to dead fiber");
    }

    status(eTransfer);
    unpack_arguments(state, args);
    invoke_context(state->vm()->thread()->vm());

    // Being cooperative...
    restart(state);

    // Through the worm hole...
    state->vm()->fiber()->suspend(state);

    // We're back...
    return return_value(state);
  }

  Object* Fiber::s_yield(STATE, Arguments& args) {
    Fiber* fiber = state->vm()->fiber();
    OnStack<1> os(state, fiber);

    if(fiber->root_p()) {
      Exception::raise_fiber_error(state, "can't yield from root fiber");
    } else if(fiber->status() == eTransfer) {
      Exception::raise_fiber_error(state, "can't yield from transferred fiber");
    }

    fiber->unpack_arguments(state, args);
    fiber->status(eYielding);

    // Being cooperative...
    fiber->invoke_context()->fiber()->restart(state);

    // Through the worm hole...
    fiber->suspend(state);

    // We're back...
    return fiber->return_value(state);
  }

  void Fiber::finalize(STATE, Fiber* fib) {
    if(state->shared().config.machine_fiber_log_finalizer.value) {
      logger::write("fiber: finalizer: %s, %d",
          fib->thread_name()->c_str(state), fib->fiber_id()->to_native());
    }
  }
}
