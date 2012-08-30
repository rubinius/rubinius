#include "builtin/thread.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/float.hpp"
#include "builtin/channel.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/fiber.hpp"
#include "builtin/location.hpp"

#include "objectmemory.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "environment.hpp"

#include "instruments/tooling.hpp"

#include "vm/object_utils.hpp"
#include "vm.hpp"

#include "on_stack.hpp"

#include "ontology.hpp"

#include "windows_compat.h"

#include <sys/time.h>

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

  Thread* Thread::create(STATE, VM* target, Object* self, Run runner,
                         bool main_thread)
  {
    Thread* thr = state->new_object<Thread>(G(thread));

    thr->thread_id(state, Fixnum::from(target->thread_id()));
    thr->alive(state, cTrue);
    thr->sleep(state, cFalse);
    thr->control_channel(state, nil<Channel>());
    thr->locals(state, LookupTable::create(state));
    thr->recursive_objects(state, LookupTable::create(state));

    thr->vm_ = target;
    thr->klass(state, as<Class>(self));
    thr->runner_ = runner;
    thr->init_lock_.init();

    target->thread.set(thr);

    if(!main_thread) thr->init_lock_.lock();

    return thr;
  }

  Object* send_run(STATE) {
    return state->vm()->thread.get()->send(state, NULL, state->symbol("__run__"));
  }

  Thread* Thread::allocate(STATE, Object* self) {
    VM* vm = state->shared().new_vm();
    Thread* thread = Thread::create(state, vm, self, send_run);

    return thread;
  }

  Thread* Thread::current(STATE) {
    return state->vm()->thread.get();
  }

  Object* Thread::unlock_locks(STATE, GCToken gct) {
    std::list<ObjectHeader*>& los = vm_->locked_objects();
    for(std::list<ObjectHeader*>::iterator i = los.begin();
        i != los.end();
        ++i) {
      (*i)->unlock_for_terminate(state, gct);
    }
    los.clear();
    return cNil;
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

  void* Thread::in_new_thread(void* ptr) {
    VM* vm = reinterpret_cast<VM*>(ptr);

    State state_obj(vm), *state = &state_obj;

    int calculate_stack = 0;
    NativeMethod::init_thread(state);
    VM::set_current(vm);

    std::ostringstream tn;
    tn << "rbx.ruby." << vm->thread_id();
    utilities::thread::Thread::set_os_name(tn.str().c_str());

    state->set_call_frame(0);
    vm->shared.gc_dependent(state);

    if(cDebugThreading) {
      std::cerr << "[THREAD " << vm->thread_id()
                << " (" << (unsigned int)thread_debug_self() << ") started thread]\n";
    }

    vm->set_root_stack(reinterpret_cast<uintptr_t>(&calculate_stack), THREAD_STACK_SIZE);

    vm->thread->init_lock_.unlock();

    vm->shared.tool_broker()->thread_start(state);
    Object* ret = vm->thread->runner_(state);
    vm->shared.tool_broker()->thread_stop(state);

    if(!ret) {
      if(vm->thread_state()->raise_reason() == cExit) {
        vm->shared.env()->halt_and_exit(state);
      }
    }

    vm->thread->init_lock_.lock();

    GCTokenImpl gct;

    std::list<ObjectHeader*>& los = vm->locked_objects();
    for(std::list<ObjectHeader*>::iterator i = los.begin();
        i != los.end();
        ++i) {
      (*i)->unlock_for_terminate(state, gct);
    }

    NativeMethod::cleanup_thread(state);

    vm->thread->alive(state, cFalse);
    vm->thread->cleanup();
    vm->thread->init_lock_.unlock();

    // Clear the call_frame, so that if we wait for GC going independent,
    // the GC doesn't see pointers into now-unallocated CallFrames
    vm->set_call_frame(0);
    vm->shared.gc_independent(state);
    vm->shared.clear_critical(state);

    VM::discard(state, vm);

    if(cDebugThreading) {
      std::cerr << "[LOCK thread " << vm->thread_id() << " exited]\n";
    }

    return 0;
  }

  Object* Thread::fork(STATE) {
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, THREAD_STACK_SIZE);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    int error = pthread_create(&vm_->os_thread(), &attrs, in_new_thread, (void*)vm_);

    if(error) {
      Exception::thread_error(state, strerror(error));
    }
    return cNil;
  }

  int Thread::fork_attached(STATE) {
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, THREAD_STACK_SIZE);

    return pthread_create(&vm_->os_thread(), &attrs, in_new_thread, (void*)vm_);
  }

  Object* Thread::pass(STATE, CallFrame* calling_environment) {
    atomic::pause();
    return cNil;
  }

  Array* Thread::list(STATE) {
    return state->shared().vm_threads(state);
  }

  Object* Thread::priority(STATE) {
    pthread_t id = vm_->os_thread();

    int _policy;
    struct sched_param params;

    if(pthread_getschedparam(id, &_policy, &params) == 0) {
      return Fixnum::from(params.sched_priority);
    }

    return cNil;
  }

  Object* Thread::raise(STATE, GCToken gct, Exception* exc) {
    init_lock_.lock();
    Thread* self = this;
    OnStack<2> os(state, self, exc);

    VM* vm = self->vm_;
    if(!vm) {
      self->init_lock_.unlock();
      return cNil;
    }

    vm->register_raise(state, exc);

    vm->wakeup(state, gct);
    self->init_lock_.unlock();
    return exc;
  }

  Object* Thread::set_priority(STATE, Fixnum* new_priority) {
    return new_priority;
  }

  Thread* Thread::wakeup(STATE, GCToken gct) {
    init_lock_.lock();
    Thread* self = this;
    OnStack<1> os(state, self);

    VM* vm = self->vm_;
    if(alive() == cFalse || !vm) {
      self->init_lock_.unlock();
      return force_as<Thread>(Primitives::failure());
    }

    vm->wakeup(state, gct);

    self->init_lock_.unlock();
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

  void Thread::cleanup() {
    vm_ = NULL;
  }

  void Thread::init_lock() {
    init_lock_.init();
  }

  Object* Thread::join(STATE, GCToken gct, CallFrame* calling_environment) {
    state->set_call_frame(calling_environment);

    init_lock_.lock();

    VM* vm = vm_;

    if(!vm) {
      init_lock_.unlock();
      return cTrue;
    }

    pthread_t id = vm->os_thread();

    if(cDebugThreading) {
      std::cerr << "[THREAD joining " << thread_debug_id(id) << "]\n";
    }

    init_lock_.unlock();

    state->gc_independent(gct);
    void* val;
    int err = pthread_join(id, &val);
    state->gc_dependent();

    switch(err) {
    case 0:
      break;
    case EDEADLK:
      std::cerr << "Join deadlock: " << thread_debug_id(id) << "/" << thread_debug_self() << "\n";
      break;
    case EINVAL:
      std::cerr << "Invalid thread id: " << thread_debug_id(id) << "\n";
      break;
    case ESRCH:
      // This means that the thread finished execution and detached
      // itself. We treat this as having joined it.
      break;
    }

    return cTrue;
  }

  Object* Thread::set_critical(STATE, Object* obj) {
    if(CBOOL(obj)) {
      state->shared().set_critical(state);
      return cTrue;
    } else {
      state->shared().clear_critical(state);
      return cFalse;
    }
  }
}
