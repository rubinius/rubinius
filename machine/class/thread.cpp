#include "memory.hpp"
#include "call_frame.hpp"
#include "environment.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "primitives.hpp"
#include "signals.hpp"
#include "thread_phase.hpp"

#include "class/channel.hpp"
#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/fiber.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/jit.hpp"
#include "class/location.hpp"
#include "class/lookup_table.hpp"
#include "class/native_method.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/thread.hpp"
#include "class/tuple.hpp"
#include "class/unwind_state.hpp"

#include "memory/collector.hpp"

#include "diagnostics/machine.hpp"

#include "dtrace/dtrace.h"

#include "logger.hpp"

#include "missing/gettid.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <sstream>
#include <regex>
#include <string>
#include <thread>

namespace rubinius {
  void Thread::bootstrap(STATE) {
    GO(thread).set(state->memory()->new_class<Class, Thread>(state, "Thread"));
  }

  Thread* Thread::create(STATE, ThreadState* thread_state) {
    return Thread::create(state, G(thread), thread_state);
  }

  Thread* Thread::create(STATE, Class* klass, ThreadState* thread_state) {
    Thread* thread = state->memory()->new_object_pinned<Thread>(state, klass);

    if(!thread_state) {
      Exception::raise_thread_error(state, "attempt to create Thread with NULL ThreadState*");
    }

    thread->thread_state(thread_state);
    thread->thread_id(state, Fixnum::from(thread_state->thread_id()));

    thread_state->set_thread(thread);

    thread->fiber(state, Fiber::create(state, thread_state));
    thread->current_fiber(state, thread->fiber());

    return thread;
  }

  Thread* Thread::create(STATE, ThreadState* thread_state, ThreadFunction function) {
    return Thread::create(state, G(thread), thread_state, function);
  }

  Thread* Thread::create(STATE, Object* self, ThreadFunction function) {
    return Thread::create(state, self, state->threads()->create_thread_state(), function);
  }

  Thread* Thread::create(STATE, Object* self, ThreadState* thread_state, ThreadFunction function) {
    Thread* thr = Thread::create(state, as<Class>(self), thread_state);

    thr->function(function);

    state->collector()->native_finalizer(state, thr,
        (memory::FinalizerFunction)&Thread::finalize);

    state->metrics()->threads_created++;

    return thr;
  }

  void Thread::finalize(STATE, Thread* thread) {
    if(state->thread() == thread) {
      // Do not finalize ourselves.
      return;
    }

    if(state->configuration()->log_thread_finalizer.value) {
      logger::write("thread: finalizer: %s", thread->thread_state()->name().c_str());
    }

    if(thread->thread_state() && thread->thread_state()->zombie_p()) {
      thread->thread_state()->discard();
      thread->thread_state(nullptr);
    }
  }

  Object* run_instance(STATE) {
    /* These are all referenced, so OnStack is not necessary. Additionally,
     * thread is pinned, so we do not need to worry about it moving.
     */
    Thread* thread = state->thread();
    Array* args = thread->args();
    Object* block = thread->block();

    if(thread->initialized()->false_p() || args->nil_p() || block->nil_p()) {
      return cNil;
    }

    Object* value = block->send(state, G(sym_call), args, block);

    /* We explicitly set the current CallFrame reference to NULL because we
     * are at the top of the stack in terms of managed code.
     */
    state->set_call_frame(NULL);

    thread->exception(state, state->unwind_state()->current_exception());

    if(state->unwind_state()->raise_reason() == cThreadKill) {
      thread->value(state, cNil);
    } else if(value) {
      thread->value(state, value);
    }

    Object* mirror = G(mirror)->send(state, state->symbol("reflect"),
        Array::from_tuple(state, Tuple::from(state, 1, thread)));
    mirror->send(state, state->symbol("finish"));

    return value;
  }

  Thread* Thread::s_new(STATE, Object* self, Array* args, Object* stack_size, Object* block) {
    Thread* thread = Thread::create(state, self, run_instance);
    OnStack<1> os(state, thread);

    if(Fixnum* size = try_as<Fixnum>(stack_size)) {
      state->validate_stack_size(state, size->to_native());
      thread->stack_size(state, size);
    }

    if(state->configuration()->log_thread_lifetime.value) {
      const std::regex& filter = state->configuration()->log_thread_filter();

      if(CallFrame* call_frame = state->get_filtered_frame(state, filter)) {
        std::ostringstream source;

        source << call_frame->file(state)->cpp_str(state).c_str()
          << ":" << call_frame->line(state);

        logger::write("thread: new: %s, %s",
            thread->thread_state()->name().c_str(), source.str().c_str());

        thread->source(state, String::create(state, source.str().c_str()));
      } else {
        logger::write("thread: new: %s", thread->thread_state()->name().c_str());
      }
    }

    if(!thread->send(state, state->symbol("initialize"), args, block, true)) {
      state->threads()->remove_thread_state(thread->thread_state());
      thread->thread_state()->set_thread_exception();
      return nullptr;
    }

    thread->fork(state);

    return thread;
  }

  Thread* Thread::s_start(STATE, Object* self, Array* args, Object* stack_size, Object* block) {
    Thread* thread = Thread::create(state, self, run_instance);
    OnStack<1> os(state, thread);

    if(Fixnum* size = try_as<Fixnum>(stack_size)) {
      state->validate_stack_size(state, size->to_native());
      thread->stack_size(state, size);
    }

    if(state->configuration()->log_thread_lifetime.value) {
      const std::regex& filter = state->configuration()->log_thread_filter();

      if(CallFrame* call_frame = state->get_filtered_frame(state, filter)) {
        std::ostringstream source;

        source << call_frame->file(state)->cpp_str(state).c_str()
          << ":" << call_frame->line(state);

        logger::write("thread: start: %s, %s",
            thread->thread_state()->name().c_str(), source.str().c_str());

        thread->source(state, String::create(state, source.str().c_str()));
      } else {
        logger::write("thread: start: %s", thread->thread_state()->name().c_str());
      }
    }

    if(!thread->send(state, state->symbol("__thread_initialize__"), args, block, true)) {
      state->threads()->remove_thread_state(thread->thread_state());
      thread->thread_state()->set_thread_exception();
      return nullptr;
    }

    thread->fork(state);

    return thread;
  }

  Thread* Thread::current(STATE) {
    return state->thread();
  }

  Object* Thread::variable_get(STATE, Symbol* key) {
    return locals()->aref(state, key);
  }

  Object* Thread::variable_set(STATE, Symbol* key, Object* value) {
    return locals()->store(state, key, value);
  }

  Object* Thread::variable_key_p(STATE, Symbol* key) {
    return locals()->has_key(state, key);
  }

  Array* Thread::variables(STATE) {
    return locals()->all_keys(state);
  }

  Array* Thread::fiber_list(STATE) {
    Array* fibers = Array::create(state, 0);

    state->threads()->each(state, [this, fibers](STATE, ThreadState* thread_state) {
        if(thread_state->kind() == ThreadState::eFiber
            && !thread_state->fiber()->nil_p()
            && thread_state->fiber()->status() != Fiber::eDead
            && thread_state->fiber()->thread() == this) {
          fibers->append(state, thread_state->fiber());
        }
      });

    return fibers;
  }

  Object* Thread::fiber_variable_get(STATE, Symbol* key) {
    return current_fiber()->locals()->aref(state, key);
  }

  Object* Thread::fiber_variable_set(STATE, Symbol* key, Object* value) {
    check_frozen(state);
    return current_fiber()->locals()->store(state, key, value);
  }

  Object* Thread::fiber_variable_key_p(STATE, Symbol* key) {
    return current_fiber()->locals()->has_key(state, key);
  }

  Array* Thread::fiber_variables(STATE) {
    return current_fiber()->locals()->all_keys(state);
  }

  Object* Thread::status(STATE) {
    return Fixnum::from(thread_state()->thread_status());
  }

  int Thread::start_thread(STATE, void* (*function)(void*)) {
    Thread* self = this;
    OnStack<1> os(state, self);

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, self->stack_size()->to_native());
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    int status = pthread_create(&self->thread_state()->os_thread(), &attrs,
        function, (void*)self->thread_state());

    pthread_attr_destroy(&attrs);

    return status;
  }

  void Thread::stop(STATE, ThreadState* thread_state) {
    // TODO: Thread
  }

  void* Thread::run(void* ptr) {
    ThreadState* state = reinterpret_cast<ThreadState*>(ptr);
    Object* value = nullptr;

    state->set_stack_bounds(state->thread()->stack_size()->to_native());
    state->set_current_thread();
    state->set_start_time();

    SET_THREAD_UNWIND(state);

    if(!state->thread_unwinding_p()) {
      RUBINIUS_THREAD_START(
          const_cast<RBX_DTRACE_CHAR_P>(state->name().c_str()), state->thread_id(), 0);

      state->thread()->pid(state, Fixnum::from(gettid()));

      if(state->configuration()->log_thread_lifetime.value) {
        logger::write("thread: run: %s, %d",
            state->name().c_str(), state->thread()->pid()->to_native());
      }

      state->metrics()->start_reporting(state);

      NativeMethod::init_thread(state);

      state->managed_phase();

      value = state->thread()->function()(state);
      state->set_call_frame(nullptr);

      {
        std::unique_lock<std::mutex> lk(state->join_lock());

        if(state->thread()->thread_state()->unwind_state()->raise_reason() == cException) {
          state->set_thread_exception();
        } else {
          state->set_thread_dead();
        }

        state->join_cond().notify_all();
      }

      if(state->lock_owned_p()) {
        logger::write("thread: exiting while owning Threads lock: %s",
            state->name().c_str());
        state->unlock();
      }
    }

    if(state->configuration()->log_thread_lifetime.value) {
      logger::write("thread: exit: %s %fs", state->name().c_str(), state->run_time());
    }

    if(state->main_thread_p()
        || (!value && state->unwind_state()->raise_reason() == cSystemExit)) {
      state->machine()->halt(state, state->unwind_state()->raise_value());
    }

    state->unmanaged_phase();

    NativeMethod::cleanup_thread(state);

    state->threads()->remove_thread_state(state);

    RUBINIUS_THREAD_STOP(
        const_cast<RBX_DTRACE_CHAR_P>(state->name().c_str()), state->thread_id(), 0);

    return 0;
  }

  Object* Thread::name(STATE) {
    return String::create(state, thread_state()->name().c_str());
  }

  Object* Thread::set_name(STATE, String* name) {
    thread_state()->set_name(state, name->c_str(state));

    return name;
  }

  void Thread::fork(STATE) {
    if(int error = start_thread(state, Thread::run)) {
      char buf[RBX_STRERROR_BUFSIZE];
      char* err = RBX_STRERROR(error, buf, RBX_STRERROR_BUFSIZE);
      Exception::raise_thread_error(state, err);
    }
  }

  Object* Thread::pass(STATE) {
    (void)std::this_thread::yield;
    return cNil;
  }

  Array* Thread::list(STATE) {
    Array* threads = Array::create(state, 0);

    state->threads()->each(state, [threads](STATE, ThreadState* thread_state) {
        Thread *thread = thread_state->thread();

        if(thread_state->kind() == ThreadState::eThread
            &&!thread->nil_p() && !thread->thread_state()->zombie_p()) {
          threads->append(state, thread);
        }
      });

    return threads;
  }

  Fixnum* Thread::count(STATE) {
    intptr_t count = 0;

    state->threads()->each(state, [&](STATE, ThreadState* thread_state) {
        Thread *thread = thread_state->thread();

        if(thread_state->kind() == ThreadState::eThread
            &&!thread->nil_p() && !thread->thread_state()->zombie_p()) {
          count++;
        }
      });

    return Fixnum::from(count);
  }

  Object* Thread::set_priority(STATE, Fixnum* new_priority) {
    priority(state, new_priority);
    return new_priority;
  }

  Object* Thread::get_priority(STATE) {
    return priority();
  }

  Object* Thread::raise(STATE, Exception* exc) {
    std::lock_guard<std::mutex> guard(thread_state()->thread_lock());

    if(thread_state()->zombie_p()) return cNil;

    current_fiber()->thread_state()->register_raise(state, exc);
    thread_state()->wakeup();

    return exc;
  }

  Object* Thread::kill(STATE) {
    std::lock_guard<std::mutex> guard(thread_state()->thread_lock());

    if(thread_state()->zombie_p()) return cNil;

    if(state->thread() == this) {
      thread_state()->unwind_state()->raise_thread_kill();
      return nullptr;
    } else {
      current_fiber()->thread_state()->register_kill(state);
      thread_state()->wakeup();
      return this;
    }
  }

  Object* Thread::suspend(STATE, Object* duration) {
    if(Fixnum* fix = try_as<Fixnum>(duration)) {
      if(!fix->positive_p()) {
        Exception::raise_argument_error(state, "time interval must be positive");
      }
    } else if(Float* flt = try_as<Float>(duration)) {
      if(flt->value() < 0.0) {
        Exception::raise_argument_error(state, "time interval must be positive");
      }
    } else if(duration == G(undefined)) {
      duration = cNil;
    } else if(!duration->nil_p()) {
      return Primitives::failure();
    }

    auto start = std::chrono::high_resolution_clock::now();

    state->thread()->thread_state()->sleep(duration);

    if(state->thread()->thread_state()->thread_interrupted_p()) {
      return nullptr;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    return Fixnum::from(elapsed.count());
  }

  Thread* Thread::wakeup(STATE) {
    std::lock_guard<std::mutex> guard(thread_state()->thread_lock());

    if(thread_state()->zombie_p()) {
      Exception::raise_thread_error(state, "attempting to wake a dead Thread");
      return nullptr;
    }

    thread_state()->wakeup();

    return this;
  }

  Tuple* Thread::context(STATE) {
    std::lock_guard<std::mutex> guard(thread_state()->thread_lock());

    if(thread_state()->zombie_p()) return nil<Tuple>();

    CallFrame* call_frame = thread_state()->get_ruby_frame();
    VariableScope* scope = call_frame->promote_scope(state);

    return Tuple::from(state, 3, Fixnum::from(call_frame->ip()),
        call_frame->compiled_code, scope);
  }

  Array* Thread::mri_backtrace(STATE) {
    std::lock_guard<std::mutex> guard(thread_state()->thread_lock());

    if(thread_state()->zombie_p()) return nil<Array>();

    return Location::mri_backtrace(state);
  }

  Thread* Thread::join(STATE, Object* timeout) {
    if(thread_state()->zombie_p()) return this;

    Thread* self = this;
    OnStack<1> os(state, self);

    // Pull this variable out before `this` may be invalid
    ThreadState* this_state = thread_state();

    {
      UnmanagedPhase unmanaged(state);
      std::unique_lock<std::mutex> lock(this_state->join_lock());

      if(timeout->nil_p()) {
        this_state->join_cond().wait(lock,
            [&]{ return this_state->zombie_p(); });
      } else {
        std::chrono::duration<double> pause(as<Float>(timeout)->value());

        if(this_state->join_cond().wait_for(lock, pause) == std::cv_status::timeout) {
          return nil<Thread>();
        }
      }
    }

    return self;
  }
}
