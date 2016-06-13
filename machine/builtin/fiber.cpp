#include "arguments.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"
#include "memory.hpp"
#include "metrics.hpp"

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

  Object* Fiber::unpack_arguments(STATE, Arguments& args) {
    switch(args.total()) {
      case 0:
        return cNil;
      case 1:
        return args.get_argument(0);
      default:
        return args.as_array(state);
    }

    // Some versions of GCC can't understand dataflow.
    return cNil;
  }

  Object* Fiber::start_fiber(STATE, Fiber* f, Arguments& args) {
    Fiber* fiber = f;
    OnStack<1> os(state, fiber);

    fiber->arguments(state, args.as_array(state));

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, fiber->stack_size()->to_native());
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    pthread_create(&fiber->vm()->os_thread(), &attrs,
        Fiber::run, (void*)fiber->vm());

    pthread_attr_destroy(&attrs);

    // Wait for Fiber thread to start up and pause.
    while(!fiber->vm()->wait_flag());

    fiber->function(Fiber::continue_fiber);

    return continue_fiber(state, fiber, args);
  }

  Object* Fiber::continue_fiber(STATE, Fiber* f, Arguments& args) {
    Fiber* fiber = f;
    OnStack<1> os(state, fiber);

    fiber->value(state, unpack_arguments(state, args));

    state->vm()->unmanaged_phase();
    state->vm()->set_wait_flag(true);

    while(fiber->vm()->wait_flag()) {
      std::lock_guard<std::mutex> guard(fiber->vm()->wait_mutex());
      fiber->vm()->wait_condition().notify_one();
    }

    {
      std::unique_lock<std::mutex> lk(state->vm()->wait_mutex());
      while(!fiber->vm()->wait_flag()) {
        state->vm()->wait_condition().wait(lk);
      }
      if(state->vm()->fiber()->status() != eTransfer) {
        state->vm()->fiber()->status(eRunning);
      }
      state->vm()->set_wait_flag(false);
    }

    state->vm()->managed_phase();

    if(state->vm()->thread()->vm()->thread_state()->current_exception()->nil_p()) {
      return fiber->value();
    } else {
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

    vm->set_wait_flag(true);

    {
      std::unique_lock<std::mutex> lk(vm->wait_mutex());
      while(!vm->fiber()->resume_context()->wait_flag()) {
        vm->wait_condition().wait(lk);
      }
      if(vm->fiber()->status() != eTransfer) {
        vm->fiber()->status(eRunning);
      }
      vm->set_wait_flag(false);
    }

    state->vm()->managed_phase();

    Object* value = vm->fiber()->block()->send(state, G(sym_call),
        vm->fiber()->arguments(), vm->fiber()->block());
    vm->set_call_frame(NULL);

    vm->fiber()->status(eDead);

    VM* resume_context = 0;

    if(value) {
      vm->fiber()->value(state, value);
      resume_context = vm->fiber()->resume_context();
    } else {
      vm->thread()->vm()->set_thread_state(vm->thread_state());
      // Usurp whatever Fiber the Thread had invoked.
      resume_context = vm->thread()->vm();
    }

    vm->unmanaged_phase();
    vm->set_wait_flag(true);

    while(resume_context->wait_flag()) {
      std::lock_guard<std::mutex> guard(resume_context->wait_mutex());
      resume_context->wait_condition().notify_one();
    }

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
    if(status() == eTransfer) {
      Exception::raise_fiber_error(state, "attempt to resume transfered fiber");
    } else if(status() == eRunning) {
      Exception::raise_fiber_error(state, "attempt to resume running fiber");
    } else if(status() == eDead) {
      Exception::raise_fiber_error(state, "attempt to resume dead fiber");
    } else if(root_p()) {
      Exception::raise_fiber_error(state, "attempt to resume root fiber");
    }

    resume_context(state->vm());
    state->vm()->thread()->vm()->set_current_fiber(vm());

    return _function_(state, this, args);
  }

  Object* Fiber::transfer(STATE, Arguments& args) {
    if(state->vm()->fiber() == this) {
      return args.as_array(state);
    }

    if(status() == eDead) {
      Exception::raise_fiber_error(state, "attempt to transfer to dead fiber");
    }

    status(eTransfer);
    resume_context(state->vm()->thread()->vm());
    state->vm()->thread()->vm()->set_current_fiber(vm());

    return _function_(state, this, args);
  }

  Object* Fiber::s_yield(STATE, Arguments& args) {
    Fiber* fiber = state->vm()->fiber();
    OnStack<1> os(state, fiber);

    if(fiber->root_p()) {
      Exception::raise_fiber_error(state, "can't yield from root fiber");
    } else if(fiber->status() == eTransfer) {
      Exception::raise_fiber_error(state, "can't yield from transferred fiber");
    }

    fiber->value(state, unpack_arguments(state, args));

    state->vm()->unmanaged_phase();
    fiber->vm()->set_wait_flag(true);

    while(fiber->resume_context()->wait_flag()) {
      std::lock_guard<std::mutex> guard(fiber->resume_context()->wait_mutex());
      fiber->resume_context()->wait_condition().notify_one();
    }

    fiber->status(eYielding);
    state->vm()->thread()->vm()->set_current_fiber(fiber->resume_context());

    {
      std::unique_lock<std::mutex> lk(state->vm()->wait_mutex());
      while(!fiber->resume_context()->wait_flag()) {
        state->vm()->wait_condition().wait(lk);
      }
      state->vm()->set_wait_flag(false);
    }

    if(fiber->status() != eTransfer) {
      fiber->status(eRunning);
    }

    state->vm()->managed_phase();

    return fiber->resume_context()->fiber()->value();
  }

  void Fiber::finalize(STATE, Fiber* fib) {
    if(state->shared().config.machine_fiber_log_finalizer.value) {
      logger::write("fiber: finalizer: %s, %d",
          fib->thread_name()->c_str(state), fib->fiber_id()->to_native());
    }
  }

  void Fiber::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);
    // Fiber* fib = force_as<Fiber>(obj);
  }
}
