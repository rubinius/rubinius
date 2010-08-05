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

#include "vm/object_utils.hpp"
#include "vm.hpp"

#include <sys/time.h>

namespace rubinius {


/* Class methods */

  void Thread::init(STATE) {
    GO(thread).set(state->new_class("Thread", G(object)));
    G(thread)->set_object_type(state, Thread::type);
  }

  Thread* Thread::create(STATE, VM* target, pthread_t tid) {
    Thread* thr = state->new_object<Thread>(G(thread));

    thr->thread_id(state, Fixnum::from(target->thread_id()));
    thr->alive(state, Qtrue);
    thr->sleep(state, Qfalse);
    thr->control_channel(state, (Channel*)Qnil);
    thr->recursive_objects(state, LookupTable::create(state));
    thr->vm_ = target;

    target->thread.set(thr);

    new(&thr->init_lock_) thread::SpinLock();

    if(tid == 0) thr->init_lock_.lock();

    return thr;
  }


/* Class primitives */

  Thread* Thread::allocate(STATE) {
    VM* vm = state->shared.new_vm();
    Thread* thread = Thread::create(state, vm);

    return thread;
  }

  Thread* Thread::current(STATE) {
    return state->thread.get();
  }


/* Instance primitives */


  void* Thread::in_new_thread(void* ptr) {
    VM* vm = reinterpret_cast<VM*>(ptr);

    int calculate_stack = 0;
    NativeMethod::init_thread(vm);
    VM::set_current(vm);

    vm->shared.gc_dependent();

    if(cDebugThreading) {
      std::cerr << "[THREAD " << pthread_self() << " started thread]\n";
    }

    vm->set_stack_bounds(reinterpret_cast<uintptr_t>(&calculate_stack), 4194304);

    vm->thread->init_lock_.unlock();

    Object* ret = vm->thread.get()->send(vm, NULL, vm->symbol("__run__"));

    if(!ret) {
      if(vm->thread_state()->raise_reason() == cExit) {
        std::cerr << "Exit from thread detected.\n";
      }
    }

    vm->thread->init_lock_.lock();

    NativeMethod::cleanup_thread(vm);

    vm->thread->cleanup();
    vm->thread->init_lock_.unlock();

    vm->shared.gc_independent();
    vm->shared.clear_critical(vm);

    VM::discard(vm);

    if(cDebugThreading) {
      std::cerr << "[LOCK thread " << pthread_self() << " exitted]\n";
    }

    return 0;
  }

  Object* Thread::fork(STATE) {
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, 4194304);

    int error = pthread_create(&vm_->os_thread_, &attrs, in_new_thread, (void*)vm_);

    if(error) {
      Exception::thread_error(state, strerror(error));
    }
    return Qnil;
  }

  Object* Thread::pass(STATE, CallFrame* calling_environment) {
    struct timespec ts = {0, 0};
    nanosleep(&ts, NULL);
    return Qnil;
  }

  Object* Thread::priority(STATE) {
    pthread_t id = vm_->os_thread_;

    if(id) {
      int _policy;
      struct sched_param params;

      pthread_check(pthread_getschedparam(id, &_policy, &params));

      return Fixnum::from(params.sched_priority);
    }

    return Qnil;
  }

  Object* Thread::raise(STATE, Exception* exc) {
    thread::SpinLock::LockGuard lg(init_lock_);

    VM* vm = vm_;
    if(!vm) return Qnil;

    vm->register_raise(state, exc);

    vm->wakeup();
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

    vm->wakeup();

    return this;
  }

  Tuple* Thread::context(STATE) {
    thread::SpinLock::LockGuard lg(init_lock_);

    VM* vm = vm_;
    if(!vm) return (Tuple*)Qnil;

    CallFrame* cf = vm->saved_call_frame();

    VariableScope* scope = cf->promote_scope(state);

    return Tuple::from(state, 3, Fixnum::from(cf->ip()), cf->cm, scope);
  }

  void Thread::detach_native_thread() {
  }

  void Thread::cleanup() {
    vm_->os_thread_ = 0;
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

    pthread_t id = vm->os_thread_;

    if(cDebugThreading) {
      std::cerr << "[THREAD joining " << id << "]\n";
    }

    init_lock_.unlock();

    state->shared.gc_independent();
    void* val;
    int err = pthread_join(id, &val);
    state->shared.gc_dependent();

    switch(err) {
    case 0:
      break;
    case EDEADLK:
      std::cerr << "Join deadlock: " << id << "/" << pthread_self() << "\n";
      break;
    case EINVAL:
      std::cerr << "Invalid thread id: " << id << "\n";
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
