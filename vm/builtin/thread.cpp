#include "builtin/channel.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fiber.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/location.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/native_method.hpp"
#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/symbol.hpp"
#include "call_frame.hpp"
#include "dtrace/dtrace.h"
#include "environment.hpp"
#include "instruments/tooling.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"
#include "on_stack.hpp"
#include "metrics.hpp"
#include "util/logger.hpp"

#include "missing/gettid.h"

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

  void Thread::init(STATE) {
    GO(thread).set(ontology::new_class(state, "Thread", G(object)));
    G(thread)->set_object_type(state, Thread::type);
  }

  Thread* Thread::create(STATE, VM* vm) {
    return Thread::create(state, G(thread), vm);
  }

  Thread* Thread::create(STATE, Class* klass, VM* vm) {
    Thread* thr = state->vm()->new_object_mature<Thread>(G(thread));

    thr->pin();
    thr->init_lock_.init();
    thr->join_lock_.init();
    thr->join_cond_.init();

    thr->vm_ = vm;
    thr->thread_id(state, Fixnum::from(vm->thread_id()));
    thr->sleep(state, cFalse);
    thr->control_channel(state, nil<Channel>());
    thr->locals(state, LookupTable::create(state));
    thr->recursive_objects(state, LookupTable::create(state));
    thr->group(state, cNil);
    thr->result(state, cFalse);
    thr->exception(state, nil<Exception>());
    thr->critical(state, cFalse);
    thr->killed(state, cFalse);
    thr->priority(state, Fixnum::from(0));
    thr->pid(state, Fixnum::from(0));
    thr->klass(state, klass);

    vm->thread.set(thr);

    return thr;
  }

  Thread* Thread::create(STATE, Object* self, ThreadFunction function) {
    VM* vm = state->shared().new_vm();
    vm->metrics().init(metrics::eRubyMetrics);

    Thread* thr = Thread::create(state, as<Class>(self), vm);

    thr->function_ = function;

    state->memory()->needs_finalization(thr, (FinalizerFunction)&Thread::finalize,
        FinalizeObject::eUnmanaged);

    state->vm()->metrics().system_metrics.vm_threads++;
    state->vm()->metrics().system_metrics.vm_threads_total++;

    return thr;
  }

  void Thread::finalize(STATE, Thread* thread) {
    if(thread->vm()->zombie_p()) {
      VM::discard(state, thread->vm());
    }
  }

  Object* send_run(STATE) {
    return state->vm()->thread.get()->send(state, NULL, state->symbol("__run__"));
  }

  Thread* Thread::allocate(STATE, Object* self) {
    return Thread::create(state, self, send_run);
  }

  Thread* Thread::current(STATE) {
    return state->vm()->thread.get();
  }

  Object* Thread::unlock_locks(STATE, GCToken gct, CallFrame* calling_environment) {
    Thread* self = this;
    OnStack<1> os(state, self);

    LockedObjects& los = self->vm_->locked_objects();
    for(LockedObjects::iterator i = los.begin();
        i != los.end();
        ++i) {
      ObjectHeader* locked = *i;
      if(locked != self) {
        locked->unlock_for_terminate(state, gct, calling_environment);
      }
    }
    los.clear();
    return cNil;
  }

  void Thread::unlock_after_fork(STATE, GCToken gct) {
    unlock_object_after_fork(state, gct);

    LockedObjects& los = vm_->locked_objects();
    for(LockedObjects::iterator i = los.begin();
        i != los.end();
        ++i) {
      Object* obj = static_cast<Object*>(*i);
      if(obj && obj != this) {
        obj->unlock_object_after_fork(state, gct);
      }
    }
    los.clear();
  }

  Object* Thread::locals_aref(STATE, Symbol* key) {
    /*
     * If we're not trying to set values on the current thread,
     * we will set thread locals anyway and not use fiber locals.
     */
    if(state->vm() != vm()) {
      return locals()->aref(state, key);
    }
    Fiber* fib = state->vm()->current_fiber.get();
    if(fib->nil_p() || fib->root_p()) {
      return locals()->aref(state, key);
    }
    if(try_as<LookupTable>(fib->locals())) {
      return fib->locals()->aref(state, key);
    }
    return cNil;
  }

  Object* Thread::locals_store(STATE, Symbol* key, Object* value) {
    /*
     * If we're not trying to set values on the current thread,
     * we will set thread locals anyway and not use fiber locals.
     */
    check_frozen(state);
    if(state->vm() != vm()) {
      return locals()->store(state, key, value);
    }
    Fiber* fib = state->vm()->current_fiber.get();
    if(fib->nil_p() || fib->root_p()) {
      return locals()->store(state, key, value);
    }
    if(fib->locals()->nil_p()) {
      fib->locals(state, LookupTable::create(state));
    }
    return fib->locals()->store(state, key, value);
  }

  Object* Thread::locals_remove(STATE, Symbol* key) {
    check_frozen(state);
    if(state->vm() != vm()) {
      return locals()->remove(state, key);
    }
    Fiber* fib = state->vm()->current_fiber.get();
    if(fib->nil_p() || fib->root_p()) {
      return locals()->remove(state, key);
    }
    if(fib->locals()->nil_p()) {
      return cNil;
    }
    return fib->locals()->remove(state, key);
  }

  Array* Thread::locals_keys(STATE) {
    /*
     * If we're not trying to set values on the current thread,
     * we will set thread locals anyway and not use fiber locals.
     */
    if(state->vm() != vm()) {
      return locals()->all_keys(state);
    }
    Fiber* fib = state->vm()->current_fiber.get();
    if(fib->nil_p() || fib->root_p()) {
      return locals()->all_keys(state);
    }
    if(try_as<LookupTable>(fib->locals())) {
      return fib->locals()->all_keys(state);
    }
    return Array::create(state, 0);
  }

  Object* Thread::locals_has_key(STATE, Symbol* key) {
    /*
     * If we're not trying to set values on the current thread,
     * we will set thread locals anyway and not use fiber locals.
     */
    if(state->vm() != vm()) {
      return locals()->has_key(state, key);
    }
    Fiber* fib = state->vm()->current_fiber.get();
    if(fib->nil_p() || fib->root_p()) {
      return locals()->has_key(state, key);
    }
    if(try_as<LookupTable>(fib->locals())) {
      return fib->locals()->has_key(state, key);
    }
    return cFalse;
  }

  int Thread::start_thread(STATE, const pthread_attr_t &attrs) {
    Thread* self = this;
    OnStack<1> os(state, self);

    self->init_lock_.lock();

    if(int error = pthread_create(&self->vm_->os_thread(),
          &attrs, Thread::run, (void*)self->vm_)) {
      return error;
    }

    // We can't return from here until the new thread completes a minimal
    // initialization. After the initialization, it unlocks init_lock_.
    // So, wait here until we can lock init_lock_ after that.
    self->init_lock_.lock();

    // We locked init_lock_. And we are sure that the new thread completed
    // the initialization.
    // Locking init_lock_ isn't needed anymore, so unlock it.
    self->init_lock_.unlock();

    return 0;
  }

  void* Thread::run(void* ptr) {
    GCTokenImpl gct;

    VM* vm = reinterpret_cast<VM*>(ptr);

    SharedState& shared = vm->shared;
    State state_obj(vm), *state = &state_obj;

    std::string thread_name;

    {
      std::ostringstream tn;
      tn << "rbx.ruby." << vm->thread_id();
      VM::set_current(vm, tn.str());
      thread_name = tn.str();
    }

    RUBINIUS_THREAD_START(const_cast<RBX_DTRACE_CHAR_P>(thread_name.c_str()),
                          vm->thread_id(), 0);

    if(cDebugThreading) {
      utilities::logger::debug("Thread: start thread: id: %d, pthread: %d",
          vm->thread_id(), (unsigned int)thread_debug_self());
    }

    int calculate_stack = 0;
    vm->set_root_stack(reinterpret_cast<uintptr_t>(&calculate_stack), THREAD_STACK_SIZE);

    NativeMethod::init_thread(state);

    vm->thread->pid(state, Fixnum::from(gettid()));

    // Lock the thread object and unlock it at __run__ in the ruby land.
    vm->thread->alive(state, cTrue);
    vm->thread->init_lock_.unlock();

    // Become GC-dependent after unlocking init_lock_ to avoid deadlocks.
    // gc_dependent may lock when it detects GC is happening. Also the parent
    // thread is locked until init_lock_ is unlocked by this child thread.
    state->gc_dependent(gct, 0);
    vm->thread->hard_lock(state, gct, 0);

    vm->shared.tool_broker()->thread_start(state);
    Object* ret = vm->thread->function_(state);
    vm->shared.tool_broker()->thread_stop(state);

    // Clear the call_frame, so that if we wait for GC going independent,
    // the GC doesn't see pointers into now-unallocated CallFrames
    vm->set_call_frame(0);

    vm->thread->join_lock_.lock();
    vm->thread->stopped();

    if(!ret) {
      if(vm->thread_state()->raise_reason() == cExit) {
        vm->shared.env()->halt_and_exit(state);
      }
    }

    LockedObjects& los = vm->locked_objects();
    for(LockedObjects::iterator i = los.begin();
        i != los.end();
        ++i) {
      (*i)->unlock_for_terminate(state, gct, 0);
    }

    vm->thread->join_cond_.broadcast();
    vm->thread->join_lock_.unlock();

    NativeMethod::cleanup_thread(state);

    if(cDebugThreading) {
      utilities::logger::debug("Thread: exit thread: id: %d", vm->thread_id());
    }

    shared.clear_critical(state);
    shared.gc_independent();

    vm->set_zombie();

    RUBINIUS_THREAD_STOP(const_cast<RBX_DTRACE_CHAR_P>(thread_name.c_str()),
                         vm->thread_id(), 0);

    return 0;
  }

  Object* Thread::fork(STATE) {
    // If the thread is already alive or already ran,
    // we can't use it anymore.
    if(CBOOL(alive()) || !vm_) {
      return Primitives::failure();
    }

    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, THREAD_STACK_SIZE);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    if(int error = start_thread(state, attrs)) {
      char buf[RBX_STRERROR_BUFSIZE];
      char* err = RBX_STRERROR(error, buf, RBX_STRERROR_BUFSIZE);
      Exception::thread_error(state, err);
    }

    pthread_attr_destroy(&attrs);

    return cNil;
  }

  Object* Thread::pass(STATE, CallFrame* calling_environment) {
    atomic::pause();
    return cNil;
  }

  Array* Thread::list(STATE) {
    return state->shared().vm_threads(state);
  }

  Object* Thread::set_priority(STATE, Fixnum* new_priority) {
    priority(state, new_priority);
    return new_priority;
  }

  Object* Thread::get_priority(STATE) {
    return priority();
  }

  Object* Thread::raise(STATE, GCToken gct, Exception* exc, CallFrame* calling_environment) {
    utilities::thread::SpinLock::LockGuard lg(init_lock_);
    Thread* self = this;
    OnStack<2> os(state, self, exc);

    VM* vm = self->vm_;
    if(!vm) {
      return cNil;
    }

    vm->register_raise(state, exc);

    vm->wakeup(state, gct, calling_environment);
    return exc;
  }

  Object* Thread::current_exception(STATE) {
    utilities::thread::SpinLock::LockGuard lg(init_lock_);
    return vm_->thread_state()->current_exception();
  }

  Object* Thread::kill(STATE, GCToken gct, CallFrame* calling_environment) {
    utilities::thread::SpinLock::LockGuard lg(init_lock_);
    Thread* self = this;
    OnStack<1> os(state, self);

    VM* vm = self->vm_;
    if(!vm) {
      return cNil;
    }

    if(state->vm()->thread.get() == self) {
      vm_->thread_state_.raise_thread_kill();
      return NULL;
    } else {
      vm->register_kill(state);
      vm->wakeup(state, gct, calling_environment);
      return self;
    }
  }

  Thread* Thread::wakeup(STATE, GCToken gct, CallFrame* calling_environment) {
    utilities::thread::SpinLock::LockGuard lg(init_lock_);
    Thread* self = this;
    OnStack<1> os(state, self);

    VM* vm = self->vm_;
    if(!CBOOL(alive()) || !vm) {
      return force_as<Thread>(Primitives::failure());
    }

    vm->wakeup(state, gct, calling_environment);

    return self;
  }

  Tuple* Thread::context(STATE) {
    utilities::thread::SpinLock::LockGuard lg(init_lock_);

    VM* vm = vm_;
    if(!vm) return nil<Tuple>();

    CallFrame* cf = vm->saved_call_frame()->top_ruby_frame();

    VariableScope* scope = cf->promote_scope(state);

    return Tuple::from(state, 3, Fixnum::from(cf->ip()), cf->compiled_code, scope);
  }

  Array* Thread::mri_backtrace(STATE, GCToken gct, CallFrame* calling_environment) {
    utilities::thread::SpinLock::LockGuard lg(init_lock_);

    VM* vm = vm_;
    if(!vm) return nil<Array>();
    StopTheWorld stop(state, gct, calling_environment);

    CallFrame* cf = vm->saved_call_frame();

    return Location::mri_backtrace(state, cf);
  }

  void Thread::stopped() {
    alive_ = cFalse;
  }

  void Thread::init_lock() {
    init_lock_.init();
  }

  Thread* Thread::join(STATE, GCToken gct, Object* timeout,
      CallFrame* calling_environment)
  {
    Thread* self = this;
    OnStack<2> os(state, self, timeout);

    state->gc_independent(gct, calling_environment);

    {
      utilities::thread::Mutex::LockGuard guard(self->join_lock_);
      state->gc_dependent(gct, calling_environment);
      atomic::memory_barrier();

      if(self->alive()->true_p()) {
        GCIndependent gc_guard(state, calling_environment);

        if(timeout->nil_p()) {
          self->join_cond_.wait(self->join_lock_);
        } else {
          struct timespec ts = {0,0};
          self->join_cond_.offset(&ts, as<Float>(timeout)->val);

          if(self->join_cond_.wait_until(self->join_lock_, &ts)
                == utilities::thread::cTimedOut) {
            return nil<Thread>();
          }
        }
      }
    }

    return self;
  }

  Object* Thread::set_critical(STATE, Object* obj, CallFrame* calling_environment) {
    state->set_call_frame(calling_environment);
    if(CBOOL(obj)) {
      state->shared().set_critical(state, calling_environment);
      return cTrue;
    } else {
      state->shared().clear_critical(state);
      return cFalse;
    }
  }
}
