#include "memory.hpp"
#include "call_frame.hpp"
#include "environment.hpp"
#include "diagnostics.hpp"
#include "metrics.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
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

  Thread* Thread::create(STATE, VM* vm) {
    return Thread::create(state, G(thread), vm);
  }

  Thread* Thread::create(STATE, Class* klass, VM* vm) {
    Thread* thr = state->memory()->new_object_pinned<Thread>(state, klass);

    if(!vm) {
      Exception::raise_thread_error(state, "attempt to create Thread with NULL VM*");
    }

    thr->vm(vm);
    thr->thread_id(state, Fixnum::from(vm->thread_id()));

    vm->set_thread(thr);

    thr->fiber(state, Fiber::create(state, vm));
    thr->current_fiber(state, thr->fiber());

    return thr;
  }

  Thread* Thread::create(STATE, VM* vm, ThreadFunction function) {
    return Thread::create(state, G(thread), vm, function);
  }

  Thread* Thread::create(STATE, Object* self, ThreadFunction function) {
    BlockPhase blocking(state);

    return Thread::create(state, self,
        state->shared().thread_nexus()->new_vm(&state->shared()),
        function);
  }

  Thread* Thread::create(STATE, Object* self, VM* vm, ThreadFunction function) {
    Thread* thr = Thread::create(state, as<Class>(self), vm);

    thr->function(function);

    state->memory()->native_finalizer(state, thr,
        (memory::FinalizerFunction)&Thread::finalize);

    state->vm()->metrics().system.threads_created++;

    return thr;
  }

  void Thread::finalize(STATE, Thread* thread) {
    if(state->shared().config.machine_thread_log_finalizer.value) {
      logger::write("thread: finalizer: %s", thread->vm()->name().c_str());
    }

    if(thread->vm() && thread->vm()->zombie_p()) {
      thread->fiber_mutex().std::mutex::~mutex();
      VM::discard(state, thread->vm());
      thread->vm(NULL);
    }
  }

  Object* run_instance(STATE) {
    /* These are all referenced, so OnStack is not necessary. Additionally,
     * thread is pinned, so we do not need to worry about it moving.
     */
    Thread* thread = state->vm()->thread();
    Array* args = thread->args();
    Object* block = thread->block();

    if(thread->initialized()->false_p() || args->nil_p() || block->nil_p()) {
      return cNil;
    }

    Object* value = block->send(state, G(sym_call), args, block);

    /* We explicitly set the current CallFrame reference to NULL because we
     * are at the top of the stack in terms of managed code.
     */
    state->vm()->set_call_frame(NULL);

    thread->exception(state, state->vm()->thread_state()->current_exception());

    if(state->vm()->thread_state()->raise_reason() == cThreadKill) {
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
      state->vm()->validate_stack_size(state, size->to_native());
      thread->stack_size(state, size);
    }

    if(state->shared().config.machine_thread_log_lifetime.value) {
      const std::regex& filter = state->shared().config.machine_thread_log_filter();

      if(CallFrame* call_frame = state->vm()->get_filtered_frame(state, filter)) {
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
      state->vm()->validate_stack_size(state, size->to_native());
      thread->stack_size(state, size);
    }

    if(state->shared().config.machine_thread_log_lifetime.value) {
      const std::regex& filter = state->shared().config.machine_thread_log_filter();

      if(CallFrame* call_frame = state->vm()->get_filtered_frame(state, filter)) {
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
    return state->vm()->thread();
  }

  void Thread::unlock_after_fork(STATE) {
    unlock_object_after_fork(state);

    memory::LockedObjects& los = vm()->locked_objects();
    for(memory::LockedObjects::iterator i = los.begin();
        i != los.end();
        ++i) {
      Object* obj = static_cast<Object*>(*i);
      if(obj && obj != this) {
        obj->unlock_object_after_fork(state);
      }
    }
    los.clear();
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
    return state->shared().vm_thread_fibers(state, this);
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
    state->vm()->managed_phase(state);

    std::string& runtime = state->shared().env()->runtime_path();

    G(rubinius)->set_const(state, "Signature",
        Integer::from(state, state->shared().env()->signature()));

    G(rubinius)->set_const(state, "RUNTIME_PATH", String::create(state,
                           runtime.c_str(), runtime.size()));

    state->vm()->thread()->pid(state, Fixnum::from(gettid()));

    state->shared().env()->load_core(state, runtime);

    state->vm()->thread_state()->clear();

    state->shared().start_console(state);
    state->shared().start_metrics(state);
    state->shared().start_diagnostics(state);
    state->shared().start_profiler(state);
    state->shared().start_jit(state);

    Object* klass = G(rubinius)->get_const(state, state->symbol("Loader"));
    if(klass->nil_p()) {
      rubinius::bug("unable to find class Rubinius::Loader");
    }

    Object* instance = 0;
    OnStack<1> os(state, instance);

    instance = klass->send(state, state->symbol("new"));
    if(instance) {
      state->shared().env()->set_loader(instance);
    } else {
      rubinius::bug("unable to instantiate Rubinius::Loader");
    }

    // Enable the JIT after the core library has loaded
    G(jit)->enable(state);

    Object* value = instance->send(state, state->symbol("main"));

    state->shared().signals()->system_exit(state->vm()->thread_state()->raise_value());

    return value;
  }

  void* Thread::run(void* ptr) {
    VM* vm = reinterpret_cast<VM*>(ptr);
    State state_obj(vm), *state = &state_obj;

    vm->set_stack_bounds(vm->thread()->stack_size()->to_native());
    vm->set_current_thread();
    vm->set_start_time();

    RUBINIUS_THREAD_START(
        const_cast<RBX_DTRACE_CHAR_P>(vm->name().c_str()), vm->thread_id(), 0);

    vm->thread()->pid(state, Fixnum::from(gettid()));

    if(state->shared().config.machine_thread_log_lifetime.value) {
      logger::write("thread: run: %s, %d, %#x",
          vm->name().c_str(), vm->thread()->pid()->to_native(),
          (unsigned int)thread_debug_self());
    }

    NativeMethod::init_thread(state);

    state->vm()->managed_phase(state);

    Object* value = vm->thread()->function()(state);
    vm->set_call_frame(NULL);

    vm->thread()->join_lock_.lock();
    vm->thread()->stopped();

    state->shared().report_profile(state);

    memory::LockedObjects& locked_objects = state->vm()->locked_objects();
    for(memory::LockedObjects::iterator i = locked_objects.begin();
        i != locked_objects.end();
        ++i)
    {
      (*i)->unlock_for_terminate(state);
    }
    locked_objects.clear();

    vm->thread()->join_cond_.broadcast();
    vm->thread()->join_lock_.unlock();

    NativeMethod::cleanup_thread(state);

    if(state->shared().config.machine_thread_log_lifetime.value) {
      logger::write("thread: exit: %s %fs", vm->name().c_str(), vm->run_time());
    }

    vm->unmanaged_phase(state);

    if(vm->main_thread_p() || (!value && vm->thread_state()->raise_reason() == cExit)) {
      state->shared().signals()->system_exit(vm->thread_state()->raise_value());
    }

    vm->set_zombie(state);

    RUBINIUS_THREAD_STOP(
        const_cast<RBX_DTRACE_CHAR_P>(vm->name().c_str()), vm->thread_id(), 0);

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
    return state->shared().vm_threads(state);
  }

  Fixnum* Thread::count(STATE) {
    return state->shared().vm_threads_count(state);
  }

  Object* Thread::set_priority(STATE, Fixnum* new_priority) {
    priority(state, new_priority);
    return new_priority;
  }

  Object* Thread::get_priority(STATE) {
    return priority();
  }

  Object* Thread::raise(STATE, Exception* exc) {
    utilities::thread::SpinLock::LockGuard guard(init_lock_);

    if(!vm()) return cNil;

    current_fiber()->vm()->register_raise(state, exc);
    current_fiber()->vm()->wakeup(state);

    return exc;
  }

  Object* Thread::kill(STATE) {
    utilities::thread::SpinLock::LockGuard guard(init_lock_);

    if(!vm()) return cNil;

    if(state->vm()->thread() == this) {
      current_fiber()->vm()->thread_state()->raise_thread_kill();
      return NULL;
    } else {
      current_fiber()->vm()->register_kill(state);
      current_fiber()->vm()->wakeup(state);
      return this;
    }
  }

  Thread* Thread::wakeup(STATE) {
    utilities::thread::SpinLock::LockGuard guard(init_lock_);

    if(!CBOOL(alive()) || !vm()) {
      return force_as<Thread>(Primitives::failure());
    }

    current_fiber()->vm()->wakeup(state);

    return this;
  }

  Tuple* Thread::context(STATE) {
    utilities::thread::SpinLock::LockGuard guard(init_lock_);

    if(!vm()) return nil<Tuple>();

    CallFrame* call_frame = vm()->get_ruby_frame();
    VariableScope* scope = call_frame->promote_scope(state);

    return Tuple::from(state, 3, Fixnum::from(call_frame->ip()),
        call_frame->compiled_code, scope);
  }

  Array* Thread::mri_backtrace(STATE) {
    utilities::thread::SpinLock::LockGuard guard(init_lock_);

    if(!vm()) return nil<Array>();

    return Location::mri_backtrace(state);
  }

  void Thread::stopped() {
    alive(cFalse);
  }

  void Thread::init_lock() {
    init_lock_.init();
  }

  Thread* Thread::join(STATE, Object* timeout) {
    if(!vm()) return nil<Thread>();

    Thread* self = this;
    OnStack<2> os(state, self, timeout);

    state->vm()->unmanaged_phase(state);

    {
      utilities::thread::Mutex::LockGuard guard(self->join_lock_);
      state->vm()->managed_phase(state);
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
