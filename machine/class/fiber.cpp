#include "arguments.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"
#include "memory.hpp"
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
#include "class/unwind_state.hpp"

#include "diagnostics/machine.hpp"

#include "memory/collector.hpp"

#include "dtrace/dtrace.h"

#include "logger.hpp"

#include "missing/gettid.h"

#include <sstream>
#include <regex>
#include <string>

namespace rubinius {
  std::atomic<uint32_t> Fiber::fiber_ids_;

  void Fiber::bootstrap(STATE) {
    GO(fiber).set(state->memory()->new_class<Class, Fiber>(state, "Fiber"));

    fiber_ids_.store(0);
  }

  bool Fiber::root_p() {
    return thread_state()->fiber()->thread_state() == thread_state()->thread()->thread_state();
  }

  bool Fiber::canceled_p(STATE) {
    return !root_p() && thread_state()->unwind_state()->raise_reason() == cFiberCancel;
  }

  void Fiber::unpack_arguments(STATE, Arguments& args) {
    switch(args.total()) {
      case 0:
        state->thread()->fiber_value(state, cNil);
        break;
      case 1:
        state->thread()->fiber_value(state, args.get_argument(0));
        break;
      default:
        state->thread()->fiber_value(state,  args.as_array(state));
        break;
    }
  }

  void Fiber::start(STATE, Arguments& args) {
    state->thread()->fiber_value(state, args.as_array(state));

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, stack_size()->to_native());
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    int status = pthread_create(&thread_state()->os_thread(), &attrs,
        Fiber::run, (void*)thread_state());

    pthread_attr_destroy(&attrs);

    if(status != 0) {
      char buf[RBX_STRERROR_BUFSIZE];
      char* err = RBX_STRERROR(status, buf, RBX_STRERROR_BUFSIZE);
      Exception::raise_fiber_error(state, err);
    }

    // Wait for Fiber thread to start up and pause.
    while(!thread_state()->suspended_p()) {
      ; // spin wait
    }
  }

  void Fiber::restart(STATE) {
    UnmanagedPhase unmanaged(state);

    {
      std::lock_guard<std::mutex> guard(state->thread()->thread_state()->fiber_mutex());

      if(!thread_state()->suspended_p()) {
        std::ostringstream msg;
        msg << "attempt to restart non-suspended ("
          << thread_state()->fiber_transition_flag() << ") fiber";
        Exception::raise_fiber_error(state, msg.str().c_str());
      }

      state->set_suspending();

      restart_context(state);
      wakeup();

      while(thread_state()->suspended_p()) {
        std::lock_guard<std::mutex> guard(thread_state()->fiber_wait_mutex());
        thread_state()->fiber_wait_condition().notify_one();
      }
    }

    while(!thread_state()->running_p()) {
      ; // spin wait
    }
  }

  void Fiber::cancel(STATE) {
    if(!thread_state()->zombie_p()) {
      thread_state()->unwind_state()->raise_fiber_cancel();

      wakeup();

      while(thread_state()->suspended_p()) {
        std::lock_guard<std::mutex> guard(thread_state()->fiber_wait_mutex());
        thread_state()->fiber_wait_condition().notify_one();
      }
    }
  }

  void Fiber::suspend_and_continue(STATE) {
    UnmanagedPhase unmanaged(state);

    {
      std::unique_lock<std::mutex> lk(thread_state()->fiber_wait_mutex());

      thread_state()->set_suspended();
      while(!wakeup_p()) {
        thread_state()->fiber_wait_condition().wait(lk);
      }
      clear_wakeup();
      thread_state()->set_resuming();
    }

    if(!thread_state()->canceled_p()) {
      std::lock_guard<std::mutex> guard(state->thread()->thread_state()->fiber_mutex());

      thread_state()->set_running();

      while(!restart_context()->suspended_p()) {
        ; // spin wait
      }

      state->thread()->current_fiber(state, this);
    }
  }

  Object* Fiber::return_value(STATE) {
    if(thread_state()->thread()->nil_p()) {
      return value();
    } else if(thread_state()->unwind_state()->raise_reason() == cNone) {
      return state->thread()->fiber_value();
    } else if(canceled_p(state)) {
      return nullptr;
    } else {
      invoke_context()->unwind_state()->set_state(thread_state()->unwind_state());
      return nullptr;
    }
  }

  void* Fiber::run(void* ptr) {
    ThreadState* state = reinterpret_cast<ThreadState*>(ptr);

    state->set_stack_bounds(state->fiber()->stack_size()->to_native());
    state->set_current_thread();
    state->set_start_time();

    SET_THREAD_UNWIND(state);

    if(!state->thread_unwinding_p()) {
      RUBINIUS_THREAD_START(
          const_cast<RBX_DTRACE_CHAR_P>(
            state->name().c_str()), state->fiber()->fiber_id()->to_native(), 0);

      state->fiber()->pid(state, Fixnum::from(gettid()));

      if(state->configuration()->log_fiber_lifetime.value) {
        logger::write("fiber: run: %s, %d, %#x",
            state->name().c_str(), state->fiber()->pid()->to_native(),
            (intptr_t)pthread_self());
      }

      NativeMethod::init_thread(state);

      state->fiber()->suspend_and_continue(state);

      Object* value = state->fiber()->block()->send(state, G(sym_call),
          as<Array>(state->thread()->fiber_value()), state->fiber()->block());
      state->set_call_frame(nullptr);

      if(value) {
        state->fiber()->value(state, value);
        state->thread()->fiber_value(state, value);
      } else {
        state->fiber()->value(state, cNil);
        state->thread()->fiber_value(state, cNil);
      }

      if(state->unwind_state()->raise_reason() != cFiberCancel) {
        if(state->fiber()->status() == eTransfer) {
          // restart the root Fiber
          state->thread()->fiber()->invoke_context(state);
          state->thread()->fiber()->restart(state);
        } else {
          state->fiber()->invoke_context()->fiber()->restart(state);
        }
      }

      {
        std::lock_guard<std::mutex> guard(state->fiber_wait_mutex());

        state->fiber()->status(eDead);
        state->set_suspended();
      }
    }

    state->unmanaged_phase();

    NativeMethod::cleanup_thread(state);

    if(state->configuration()->log_fiber_lifetime.value) {
      logger::write("fiber: exit: %s %fs", state->name().c_str(), state->run_time());
    }

    state->threads()->remove_thread_state(state);
    state->set_thread_dead();

    RUBINIUS_THREAD_STOP(
        const_cast<RBX_DTRACE_CHAR_P>(
          state->name().c_str()), state->fiber()->fiber_id()->to_native(), 0);

    return 0;
  }

  Fiber* Fiber::current(STATE) {
    return state->fiber();
  }

  /* This creates the pseudo-Fiber for the Thread. This provides a uniform
   * means of expressing things like Fiber.current.
   */
  Fiber* Fiber::create(STATE, ThreadState* thread_state) {
    Fiber* fiber = state->memory()->new_object_pinned<Fiber>(state, G(fiber));

    thread_state->set_fiber(fiber);
    thread_state->set_running();

    fiber->thread_state(thread_state);
    fiber->pid(thread_state->thread()->pid());
    fiber->stack_size(thread_state->thread()->stack_size());
    fiber->thread(thread_state->thread());
    fiber->thread_name(state, String::create(state, thread_state->name().c_str()));
    fiber->fiber_id(Fixnum::from(0));
    fiber->source(state, thread_state->thread()->source());
    fiber->status(eRunning);
    fiber->invoke_context(thread_state);

    return fiber;
  }

  Fiber* Fiber::create(STATE, Object* self, Object* stack_size, Object* block) {
    Fiber* fiber = state->memory()->new_object_pinned<Fiber>(state, as<Class>(self));
    fiber->block(state, block);

    std::ostringstream name;
    name << "fiber." << fiber->fiber_id()->to_native();

    fiber->thread_state(state->threads()->create_thread_state(name.str().c_str()));

    fiber->thread_state()->set_kind(ThreadState::eFiber);
    fiber->thread_state()->set_suspending();

    if(Fixnum* size = try_as<Fixnum>(stack_size)) {
      fiber->thread_state()->validate_stack_size(state, size->to_native());
      fiber->stack_size(state, size);
    }

    fiber->thread_state()->set_thread(state->thread());
    fiber->thread_state()->set_fiber(fiber);

    fiber->thread(state->thread());
    fiber->thread_name(state, String::create(state, state->name().c_str()));

    state->collector()->native_finalizer(state, fiber,
        (memory::FinalizerFunction)&Fiber::finalize);

    if(state->configuration()->log_fiber_lifetime.value) {
      const std::regex& filter = state->configuration()->log_fiber_filter();

      if(CallFrame* call_frame = state->get_filtered_frame(state, filter)) {
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

    state->metrics()->fibers_created++;

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
      std::lock_guard<std::mutex> guard(state->thread()->thread_state()->fiber_mutex());

      if(state->thread() != thread()) {
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
      invoke_context(state);

      if(status() == eCreated) {
        start(state, args);
      }

      status(eRunning);
    }

    // Being cooperative...
    restart(state);

    // Through the worm hole...
    state->fiber()->suspend_and_continue(state);

    // We're back...
    return return_value(state);
  }

  Object* Fiber::transfer(STATE, Arguments& args) {
    {
      std::lock_guard<std::mutex> guard(state->thread()->thread_state()->fiber_mutex());

      if(state->thread() != thread()) {
        std::ostringstream msg;

        msg << "attempt to transfer fiber across threads: current thread: "
            << state->thread()->thread_id()->to_native()
            << ", Fiber Thread: ";

        if(thread()->nil_p()) {
          msg << "nil";
        } else {
          msg << thread()->thread_id()->to_native();
        }

        Exception::raise_fiber_error(state, msg.str().c_str());
      } else if(status() == eDead) {
        Exception::raise_fiber_error(state, "attempt to transfer to dead fiber");
      } else if(state->fiber() == this) {
        // This should arguably be a FiberError
        return args.as_array(state);
      }

      unpack_arguments(state, args);
      invoke_context(state);

      if(status() == eCreated) {
        start(state, args);
      }

      status(eTransfer);
    }

    // Being cooperative...
    restart(state);

    // Through the worm hole...
    state->fiber()->suspend_and_continue(state);

    // We're back...
    return return_value(state);
  }

  Object* Fiber::dispose(STATE) {
    cancel(state);

    return this;
  }

  Object* Fiber::s_yield(STATE, Arguments& args) {
    Fiber* fiber = state->fiber();
    UnwindState* unwind_state = state->unwind_state();
    OnStack<2> os(state, fiber, unwind_state);

    {
      std::lock_guard<std::mutex> guard(state->thread()->thread_state()->fiber_mutex());

      if(fiber->root_p()) {
        Exception::raise_fiber_error(state, "can't yield from root fiber");
      } else if(fiber->status() == eTransfer) {
        Exception::raise_fiber_error(state, "can't yield from transferred fiber");
      }

      fiber->unpack_arguments(state, args);
      fiber->status(eYielding);
    }

    // Being cooperative...
    state->unwind_state()->clear_raise();
    fiber->invoke_context()->fiber()->restart(state);

    // Through the worm hole...
    fiber->suspend_and_continue(state);

    if(fiber->canceled_p(state)) return nullptr;

    if(!unwind_state->nil_p()) {
      state->unwind_state()->set_state(unwind_state);
    }

    // We're back...
    return state->thread()->fiber_value();
  }

  Array* Fiber::s_list(STATE) {
    Array* fibers = Array::create(state, 0);

    state->threads()->each(state, [fibers](STATE, ThreadState* thread_state) {
        if(thread_state->kind() == ThreadState::eFiber
            && !thread_state->fiber()->nil_p()
            && thread_state->fiber()->status() != Fiber::eDead) {
          fibers->append(state, thread_state->fiber());
        }
      });

    return fibers;
  }

  Fiber* Fiber::s_main(STATE) {
    return state->thread()->fiber();
  }

  Fixnum* Fiber::s_count(STATE) {
    intptr_t count = 0;

    state->threads()->each(state, [&](STATE, ThreadState* thread_state) {
        if(thread_state->kind() == ThreadState::eFiber
            && !thread_state->fiber()->nil_p()
            && thread_state->fiber()->status() != Fiber::eDead) {
          count++;
        }
      });

    return Fixnum::from(count);
  }

  void Fiber::finalize(STATE, Fiber* fiber) {
    if(state->configuration()->log_fiber_finalizer.value) {
      logger::write("fiber: finalizer: %s, %d",
          fiber->thread_name()->c_str(state), fiber->fiber_id()->to_native());
    }

    if(fiber->thread_state()) {
      if(!state->machine()->machine_state()->halting_p()) {
        if(!fiber->thread_state()->zombie_p()) {
          fiber->cancel(state);
        }
      }

      if(fiber->thread_state()->zombie_p()) {
        fiber->thread_state()->discard();
        fiber->thread_state(nullptr);
      } else {
        logger::write("fiber: finalizer: fiber not completed: %s, %d",
            fiber->thread_name()->c_str(state), fiber->fiber_id()->to_native());
      }
    }
  }
}
