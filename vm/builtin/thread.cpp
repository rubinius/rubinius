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

#include "objectmemory.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "environment.hpp"

#include "instruments/tooling.hpp"

#include "vm/object_utils.hpp"
#include "vm.hpp"

#include "on_stack.hpp"

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
    GO(thread).set(state->new_class("Thread", G(object)));
    G(thread)->set_object_type(state, Thread::type);
  }

  Thread* Thread::create(STATE, VM* target, Object* self, Run runner,
                         bool main_thread)
  {
    Thread* thr = state->new_object<Thread>(G(thread));

    thr->thread_id(state, Fixnum::from(target->thread_id()));
    thr->alive(state, Qtrue);
    thr->sleep(state, Qfalse);
    thr->control_channel(state, nil<Channel>());
    thr->recursive_objects(state, LookupTable::create(state));
    thr->vm_ = target;
    thr->klass(state, as<Class>(self));
    thr->runner_ = runner;

    target->thread.set(thr);

    new(&thr->init_lock_) thread::SpinLock();

    if(!main_thread) thr->init_lock_.lock();

    return thr;
  }

  Object* send_run(STATE) {
    return state->thread.get()->send(state, NULL, state->symbol("__run__"));
  }

  Thread* Thread::allocate(STATE, Object* self) {
    VM* vm = state->shared.new_vm();
    Thread* thread = Thread::create(state, vm, self, send_run);

    return thread;
  }

  Thread* Thread::current(STATE) {
    return state->thread.get();
  }

  Object* Thread::unlock_locks(STATE) {
    std::list<ObjectHeader*>& los = vm_->locked_objects();
    for(std::list<ObjectHeader*>::iterator i = los.begin();
        i != los.end();
        i++) {
      (*i)->unlock_for_terminate(vm_);
    }
    los.clear();
    return Qnil;
  }

  void* Thread::in_new_thread(void* ptr) {
    VM* vm = reinterpret_cast<VM*>(ptr);

    int calculate_stack = 0;
    NativeMethod::init_thread(vm);
    VM::set_current(vm);

    vm->set_call_frame(0);
    vm->shared.gc_dependent(vm);

    if(cDebugThreading) {
      std::cerr << "[THREAD " << vm->thread_id()
                << " (" << (unsigned int)thread_debug_self() << ") started thread]\n";
    }

    vm->set_stack_bounds(reinterpret_cast<uintptr_t>(&calculate_stack), 4194304);

    vm->thread->init_lock_.unlock();

    vm->shared.tool_broker()->thread_start(vm);
    Object* ret = vm->thread->runner_(vm);
    vm->shared.tool_broker()->thread_stop(vm);

    if(!ret) {
      if(vm->thread_state()->raise_reason() == cExit) {
        vm->shared.env()->halt();
      }
    }

    vm->thread->init_lock_.lock();

    std::list<ObjectHeader*>& los = vm->locked_objects();
    for(std::list<ObjectHeader*>::iterator i = los.begin();
        i != los.end();
        i++) {
      (*i)->unlock_for_terminate(vm);
    }

    NativeMethod::cleanup_thread(vm);

    vm->thread->cleanup();
    vm->thread->init_lock_.unlock();

    vm->shared.remove_managed_thread(vm);

    // Clear the call_frame, so that if we wait for GC going independent,
    // the GC doesn't see pointers into now-unallocated CallFrames
    vm->set_call_frame(0);
    vm->shared.gc_independent(vm);
    vm->shared.clear_critical(vm);

    VM::discard(vm, vm);

    if(cDebugThreading) {
      std::cerr << "[LOCK thread " << vm->thread_id() << " exited]\n";
    }

    return 0;
  }

  Object* Thread::fork(STATE) {
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, 4194304);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    int error = pthread_create(&vm_->os_thread(), &attrs, in_new_thread, (void*)vm_);

    if(error) {
      Exception::thread_error(state, strerror(error));
    }
    return Qnil;
  }

  int Thread::fork_attached(STATE) {
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, 4194304);

    return pthread_create(&vm_->os_thread(), &attrs, in_new_thread, (void*)vm_);
  }

  Object* Thread::pass(STATE, CallFrame* calling_environment) {
    struct timespec ts = {0, 0};
    nanosleep(&ts, NULL);
    return Qnil;
  }

  Object* Thread::priority(STATE) {
    pthread_t id = vm_->os_thread();

    int _policy;
    struct sched_param params;

    if(pthread_getschedparam(id, &_policy, &params) == 0) {
      return Fixnum::from(params.sched_priority);
    }

    return Qnil;
  }

  Object* Thread::raise(STATE, Exception* exc) {
    OnStack<1> os(state, exc);

    thread::SpinLock::LockGuard lg(init_lock_);

    VM* vm = vm_;
    if(!vm) return Qnil;

    vm->register_raise(state, exc);

    vm->wakeup(state);
    return exc;
  }

  Object* Thread::set_priority(STATE, Fixnum* new_priority) {
    return new_priority;
  }

  Thread* Thread::wakeup(STATE) {
    thread::SpinLock::LockGuard lg(init_lock_);

    if(alive() == Qfalse || !vm_) {
      return reinterpret_cast<Thread*>(kPrimitiveFailed);
    }

    VM* vm = vm_;
    if(!vm) return this;

    vm->wakeup(state);

    return this;
  }

  Tuple* Thread::context(STATE) {
    thread::SpinLock::LockGuard lg(init_lock_);

    VM* vm = vm_;
    if(!vm) return nil<Tuple>();

    CallFrame* cf = vm->saved_call_frame()->top_ruby_frame();

    VariableScope* scope = cf->promote_scope(state);

    return Tuple::from(state, 3, Fixnum::from(cf->ip()), cf->cm, scope);
  }

  void Thread::cleanup() {
    vm_ = NULL;
  }

  Object* Thread::join(STATE, CallFrame* calling_environment) {
    state->set_call_frame(calling_environment);

    init_lock_.lock();

    VM* vm = vm_;

    if(!vm) {
      init_lock_.unlock();
      return Qtrue;
    }

    pthread_t id = vm->os_thread();

    if(cDebugThreading) {
      std::cerr << "[THREAD joining " << thread_debug_id(id) << "]\n";
    }

    init_lock_.unlock();

    state->shared.gc_independent(state);
    void* val;
    int err = pthread_join(id, &val);
    state->shared.gc_dependent(state);

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

    return Qtrue;
  }

  Object* Thread::set_critical(STATE, Object* obj) {
    if(RTEST(obj)) {
      state->shared.set_critical(state);
      return Qtrue;
    } else {
      state->shared.clear_critical(state);
      return Qfalse;
    }
  }
}
