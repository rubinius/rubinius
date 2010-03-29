#include "builtin/thread.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/float.hpp"
#include "builtin/channel.hpp"

#include "objectmemory.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"

#include "vm/object_utils.hpp"
#include "vm.hpp"

#include "native_thread.hpp"

#include <sys/time.h>

namespace rubinius {


/* Class methods */

  void Thread::init(STATE) {
    GO(thread).set(state->new_class("Thread", G(object)));
    G(thread)->set_object_type(state, Thread::type);
  }

  Thread* Thread::create(STATE, VM* target, pthread_t tid) {
    Thread* thr = state->new_object<Thread>(G(thread));

    thr->alive(state, Qtrue);
    thr->sleep(state, Qfalse);
    thr->control_channel(state, (Channel*)Qnil);
    thr->recursive_objects(state, LookupTable::create(state));

    target->thread.set(thr);
    thr->native_thread_ = new NativeThread(target, 4194304, tid);

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

  Object* Thread::fork(STATE) {
    state->shared.enable_preemption();

    assert(native_thread_);
    // Let it run.
    int error = native_thread_->run();
    if(error) {
      Exception::thread_error(state, strerror(error));
    }
    return Qnil;
  }

  Object* Thread::pass(STATE) {
    GlobalLock::UnlockGuard x(state->global_lock());

    return Qnil;
  }

  Object* Thread::priority(STATE) {
    if(!native_thread_) return Qnil;
    return Fixnum::from(native_thread_->priority());
  }

  Object* Thread::raise(STATE, Exception* exc) {
    // vm is NULL if the thread has exitted.
    if(!native_thread_) return Qnil;
    VM* vm = native_thread_->vm();
    thread::Mutex::LockGuard x(vm->local_lock());
    vm->register_raise(exc);
    vm->wakeup();
    return exc;
  }

  Object* Thread::set_priority(STATE, Fixnum* new_priority) {
    if(!native_thread_) return Qnil;
    native_thread_->set_priority(new_priority->to_native());
    return new_priority;
  }

  Thread* Thread::wakeup(STATE) {
    if(alive() == Qfalse || !native_thread_) {
      return reinterpret_cast<Thread*>(kPrimitiveFailed);
    }

    VM* vm = native_thread_->vm();
    {
      thread::Mutex::LockGuard x(vm->local_lock());
      vm->wakeup();
    }

    return this;
  }

  Tuple* Thread::context(STATE) {
    if(!native_thread_) return (Tuple*)Qnil;

    VM* vm = native_thread_->vm();
    CallFrame* cf = vm->saved_call_frame();

    VariableScope* scope = cf->promote_scope(state);

    return Tuple::from(state, 3, Fixnum::from(cf->ip()), cf->cm, scope);
  }

  void Thread::detach_native_thread() {
    native_thread_->detach();
    native_thread_ = NULL;
  }
}
