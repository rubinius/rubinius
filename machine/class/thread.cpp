#include "memory.hpp"
#include "call_frame.hpp"
#include "environment.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "primitives.hpp"
#include "signal.hpp"
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

#include <ostream>
#include <regex>
#include <string>

/* HACK: returns a value that should identify a native thread
 * for debugging threading issues. The winpthreads library
 * defines pthread_t to be a structure not a pointer.
 */
intptr_t thread_debug_self() {
#ifdef RBX_WINDOWS
  return (intptr_t)(pthread_self()).p;
#else
  return (intptr_t)pthread_self();
#endif
}

static intptr_t thread_debug_id(pthread_t thr) {
#ifdef RBX_WINDOWS
    return (intptr_t)thr.p;
#else
    return (intptr_t)thr;
#endif
}


namespace rubinius {

  void Thread::bootstrap(STATE) {
    GO(thread).set(state->memory()->new_class<Class, Thread>(state, "Thread"));
  }

  Thread* Thread::create(STATE, ThreadState* vm) {
    return Thread::create(state, G(thread), vm);
  }

  Thread* Thread::create(STATE, Class* klass, ThreadState* vm) {
    Thread* thr = state->memory()->new_object_pinned<Thread>(state, klass);

    if(!vm) {
      Exception::raise_thread_error(state, "attempt to create Thread with NULL ThreadState*");
    }

    thr->vm(vm);
    thr->thread_id(state, Fixnum::from(vm->thread_id()));

    vm->set_thread(thr);

    thr->fiber(state, Fiber::create(state, vm));
    thr->current_fiber(state, thr->fiber());

    return thr;
  }

  Thread* Thread::create(STATE, ThreadState* vm, ThreadFunction function) {
    return Thread::create(state, G(thread), vm, function);
  }

  Thread* Thread::create(STATE, Object* self, ThreadFunction function) {
    return Thread::create(state, self,
        state->thread_nexus()->thread_state(state->machine()),
        function);
  }

  Thread* Thread::create(STATE, Object* self, ThreadState* vm, ThreadFunction function) {
    Thread* thr = Thread::create(state, as<Class>(self), vm);

    thr->function(function);

    state->collector()->native_finalizer(state, thr,
        (memory::FinalizerFunction)&Thread::finalize);

    state->metrics()->threads_created++;

    return thr;
  }

  void Thread::finalize(STATE, Thread* thread) {
    if(state->configuration()->log_thread_finalizer.value) {
      logger::write("thread: finalizer: %s", thread->vm()->name().c_str());
    }

    if(thread->vm() && thread->vm()->zombie_p()) {
      thread->fiber_mutex().std::mutex::~mutex();
      ThreadState::discard(state, thread->vm());
      thread->vm(NULL);
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
            thread->vm()->name().c_str(), source.str().c_str());

        thread->source(state, String::create(state, source.str().c_str()));
      } else {
        logger::write("thread: new: %s", thread->vm()->name().c_str());
      }
    }

    if(!thread->send(state, state->symbol("initialize"), args, block, true)) {
      thread->vm()->set_zombie(state);
      return NULL;
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
            thread->vm()->name().c_str(), source.str().c_str());

        thread->source(state, String::create(state, source.str().c_str()));
      } else {
        logger::write("thread: start: %s", thread->vm()->name().c_str());
      }
    }

    if(!thread->send(state, state->symbol("__thread_initialize__"), args, block, true)) {
      thread->vm()->set_zombie(state);
      return NULL;
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
    return state->machine()->vm_thread_fibers(state, this);
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

  int Thread::start_thread(STATE, void* (*function)(void*)) {
    Thread* self = this;
    OnStack<1> os(state, self);

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, self->stack_size()->to_native());
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    int status = pthread_create(&self->vm()->os_thread(), &attrs,
        function, (void*)self->vm());

    pthread_attr_destroy(&attrs);

    return status;
  }

  Object* Thread::main_thread(STATE) {
    state->managed_phase(state);

    state->thread()->pid(state, Fixnum::from(gettid()));

    state->environment()->load_core(state);

    state->unwind_state()->clear();

    Object* klass = G(rubinius)->get_const(state, state->symbol("Loader"));
    if(klass->nil_p()) {
      state->environment()->missing_core("unable to find class Rubinius::Loader");
      return 0;
    }

    Object* instance = 0;
    OnStack<1> os(state, instance);

    instance = klass->send(state, state->symbol("new"));
    if(instance) {
      state->environment()->set_loader(instance);
    } else {
      state->environment()->missing_core("unable to instantiate Rubinius::Loader");
      return 0;
    }

    // Enable the JIT after the core library has loaded
    G(jit)->enable(state);

    Object* value = instance->send(state, state->symbol("main"));

    state->machine()->signals()->system_exit(state->unwind_state()->raise_value());

    return value;
  }

  void* Thread::run(void* ptr) {
    ThreadState* state = reinterpret_cast<ThreadState*>(ptr);

    state->set_stack_bounds(state->thread()->stack_size()->to_native());
    state->set_current_thread();
    state->set_start_time();

    RUBINIUS_THREAD_START(
        const_cast<RBX_DTRACE_CHAR_P>(state->name().c_str()), state->thread_id(), 0);

    state->thread()->pid(state, Fixnum::from(gettid()));

    if(state->configuration()->log_thread_lifetime.value) {
      logger::write("thread: run: %s, %d, %#x",
          state->name().c_str(), state->thread()->pid()->to_native(),
          (unsigned int)thread_debug_self());
    }

    state->metrics()->start_reporting(state);

    NativeMethod::init_thread(state);

    state->managed_phase(state);

    Object* value = state->thread()->function()(state);
    state->set_call_frame(NULL);

    state->thread()->join_lock_.lock();
    state->thread()->stopped();

    state->thread()->join_cond_.broadcast();
    state->thread()->join_lock_.unlock();

    NativeMethod::cleanup_thread(state);

    if(state->thread_nexus()->lock_owned_p(state)) {
      logger::write("thread: exiting while owning ThreadNexus lock: %s", state->name().c_str());
      state->thread_nexus()->unlock(state, state);
    }

    if(state->configuration()->log_thread_lifetime.value) {
      logger::write("thread: exit: %s %fs", state->name().c_str(), state->run_time());
    }

    state->unmanaged_phase(state);

    if(state->main_thread_p() || (!value && state->unwind_state()->raise_reason() == cExit)) {
      state->machine()->signals()->system_exit(state->unwind_state()->raise_value());
    }

    state->set_zombie(state);

    RUBINIUS_THREAD_STOP(
        const_cast<RBX_DTRACE_CHAR_P>(state->name().c_str()), state->thread_id(), 0);

    return 0;
  }

  Object* Thread::name(STATE) {
    return String::create(state, vm()->name().c_str());
  }

  Object* Thread::set_name(STATE, String* name) {
    vm()->set_name(state, name->c_str(state));

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
    atomic::pause();
    return cNil;
  }

  Array* Thread::list(STATE) {
    return state->machine()->vm_threads(state);
  }

  Fixnum* Thread::count(STATE) {
    return state->machine()->vm_threads_count(state);
  }

  Object* Thread::set_priority(STATE, Fixnum* new_priority) {
    priority(state, new_priority);
    return new_priority;
  }

  Object* Thread::get_priority(STATE) {
    return priority();
  }

  Object* Thread::raise(STATE, Exception* exc) {
    utilities::thread::SpinLock::LockGuard guard(lock_);

    if(!vm()) return cNil;

    current_fiber()->vm()->register_raise(state, exc);
    current_fiber()->vm()->wakeup(state);

    return exc;
  }

  Object* Thread::kill(STATE) {
    utilities::thread::SpinLock::LockGuard guard(lock_);

    if(!vm()) return cNil;

    if(state->thread() == this) {
      current_fiber()->vm()->unwind_state()->raise_thread_kill();
      return NULL;
    } else {
      current_fiber()->vm()->register_kill(state);
      current_fiber()->vm()->wakeup(state);
      return this;
    }
  }

  Thread* Thread::wakeup(STATE) {
    utilities::thread::SpinLock::LockGuard guard(lock_);

    if(!CBOOL(alive()) || !vm()) {
      return force_as<Thread>(Primitives::failure());
    }

    current_fiber()->vm()->wakeup(state);

    return this;
  }

  Tuple* Thread::context(STATE) {
    utilities::thread::SpinLock::LockGuard guard(lock_);

    if(!vm()) return nil<Tuple>();

    CallFrame* call_frame = vm()->get_ruby_frame();
    VariableScope* scope = call_frame->promote_scope(state);

    return Tuple::from(state, 3, Fixnum::from(call_frame->ip()),
        call_frame->compiled_code, scope);
  }

  Array* Thread::mri_backtrace(STATE) {
    utilities::thread::SpinLock::LockGuard guard(lock_);

    if(!vm()) return nil<Array>();

    return Location::mri_backtrace(state);
  }

  void Thread::stopped() {
    alive(cFalse);
  }

  Thread* Thread::join(STATE, Object* timeout) {
    if(!vm()) return nil<Thread>();

    Thread* self = this;
    OnStack<2> os(state, self, timeout);

    state->unmanaged_phase(state);

    {
      utilities::thread::Mutex::LockGuard guard(self->join_lock_);
      state->managed_phase(state);
      atomic::memory_barrier();

      if(self->alive()->true_p()) {
        UnmanagedPhase unmanaged(state);

        if(timeout->nil_p()) {
          self->join_cond_.wait(self->join_lock_);
        } else {
          struct timespec ts = {0,0};
          self->join_cond_.offset(&ts, as<Float>(timeout)->value());

          if(self->join_cond_.wait_until(self->join_lock_, &ts)
                == utilities::thread::cTimedOut) {
            return nil<Thread>();
          }
        }
      }
    }

    return self;
  }
}
