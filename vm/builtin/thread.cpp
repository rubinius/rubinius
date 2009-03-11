#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/float.hpp"

#include "objectmemory.hpp"
#include "message.hpp"

#include "vm/object_utils.hpp"
#include "vm.hpp"

#include "native_thread.hpp"

#include <sys/time.h>

namespace rubinius {

  void Thread::init(STATE) {
    GO(thread).set(state->new_class("Thread", G(object)));
    G(thread)->set_object_type(state, Thread::type);
  }


  Object* Thread::set_priority(STATE, Fixnum* new_priority) {
    native_thread_->set_priority(new_priority->to_native());
    return new_priority;
  }

  Object* Thread::priority(STATE) {
    return Fixnum::from(native_thread_->priority());
  }

  Thread* Thread::create(STATE, VM* target) {
    Thread* thr = state->new_object<Thread>(G(thread));

    thr->alive(state, Qtrue);
    thr->queued(state, Qfalse);
    thr->sleep(state, Qfalse);
    thr->frozen_stack(state, Qfalse);

    target->thread.set(thr);
    thr->vm = target;
    thr->native_thread_ = new NativeThread(target);

    return thr;
  }

  /**
   *  @todo Causes a problem with call frames. This does not seem to
   *        be performing a full setup on the VM (Environment does
   *        VM::initialize and eventually VM::boot.) It is not defined
   *        whether VM owns Threads or the other way around.
   */
  Thread* Thread::allocate(STATE) {
    VM* vm = state->shared.new_vm();
    Thread* thread = Thread::create(state, vm);

    return thread;
  }

  /* Primitives */

  Object* Thread::fork(STATE) {
    NativeThread* nt = native_thread();

    // Let it run.
    nt->run();
    return Qnil;
  }

  Thread* Thread::current(STATE) {
    return state->thread.get();
  }

  Object* Thread::pass(STATE) {
    GlobalLock::UnlockGuard x(state->global_lock());
    sched_yield();

    return Qnil;
  }

  Thread* Thread::wakeup(STATE) {
    if(alive() == Qfalse) {
      return reinterpret_cast<Thread*>(kPrimitiveFailed);
    }

    {
      thread::Mutex::LockGuard x(vm->local_lock());
      vm->wakeup();
    }

    return this;
  }

  Object* Thread::raise(STATE, Exception* exc) {
    thread::Mutex::LockGuard x(vm->local_lock());
    vm->register_raise(exc);
    vm->wakeup();
    return exc;
  }

}
