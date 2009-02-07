#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/contexts.hpp"
#include "builtin/channel.hpp"
#include "builtin/float.hpp"

#include "objectmemory.hpp"
#include "message.hpp"

#include "vm/object_utils.hpp"
#include "vm.hpp"

#include "native_thread.hpp"

#include <sys/time.h>

namespace rubinius {

  void Thread::init(STATE) {
    Tuple* tup = Tuple::from(state, 3,
                             List::create(state),
                             List::create(state),
                             List::create(state) );

    GO(scheduled_threads).set(tup);

    GO(thread).set(state->new_class("Thread", G(object)));
    G(thread)->set_object_type(state, Thread::type);

    G(thread)->set_const(state, "ScheduledThreads", tup);
  }


/* Accessor implementation */

  /** @todo   Should we queue thread? Probably unnecessary. --rue */
  void Thread::priority(STATE, Fixnum* new_priority) {
    /* This gets somewhat ugly to avoid existing lists. */
    if(new_priority->to_native() < 0) {
      Exception::argument_error(state, "Thread priority must be non-negative!");
    }

    Tuple* scheduled = state->globals.scheduled_threads.get();

    std::size_t desired = new_priority->to_ulong();
    std::size_t existing = scheduled->num_fields();

    if(desired >= existing) {
      Tuple* replacement = Tuple::create(state, (desired + 1));
      replacement->copy_from(state, scheduled, Fixnum::from(0),
			     Fixnum::from(scheduled->num_fields()),
			     Fixnum::from(0));

      for(std::size_t i = existing - 1; i <= desired; ++i) {
        if(replacement->at(state, i)->nil_p()) {
          replacement->put(state, i, List::create(state));
        }
      }

      state->globals.scheduled_threads.set(replacement);
      scheduled = replacement;
    }

    priority_ = new_priority;
  }

  Thread* Thread::create(STATE, VM* target) {
    Thread* thr = state->new_object<Thread>(G(thread));

    thr->alive(state, Qtrue);
    thr->channel(state, reinterpret_cast<Channel*>(Qnil));
    thr->priority(state, Fixnum::from(2));
    thr->queued(state, Qfalse);
    thr->sleep(state, Qtrue);
    thr->frozen_stack(state, Qfalse);

    thr->boot_task(state);

    target->thread.set(thr);
    thr->vm = target;
    thr->native_thread_ = new NativeThread(target);

    return thr;
  }

  Thread* Thread::s_new(STATE, Message& msg) {
    VM* vm = state->shared.new_vm();
    Thread* thread = Thread::create(state, vm);

    NativeThread* nt = thread->native_thread();
    nt->set_startup(msg.block, msg.as_array(state));
    // Let it run.

    {
      GlobalLock::UnlockGuard x(state->global_lock());
      nt->run();
    }

    return thread;
  }

  /* Primitives */

  Thread* Thread::current(STATE) {
    return state->thread.get();
  }

  /** @todo   Add voluntary/involuntary? --rue */
  Object* Thread::exited(STATE) {
    alive(state, Qfalse);

    if(!channel()->nil_p()) {
      channel()->cancel_waiter(state, this);
    }

    channel(state, (Channel*)Qnil);

    state->dequeue_thread(this);
    state->find_and_activate_thread();

    return this;
  }

  Object* Thread::pass(STATE) {
    {
      GlobalLock::UnlockGuard x(state->global_lock());
      sched_yield();
    }

    std::cout << "unpassed!\n";
    return Qnil;
  }

  Object* Thread::raise(STATE, Exception* error) {
    wakeup(state);

    return task_->raise(state, error);
  }

  Thread* Thread::wakeup(STATE) {
    if(alive() == Qfalse) {
      return reinterpret_cast<Thread*>(kPrimitiveFailed);
    }

    {
      thread::Mutex::LockGuard x(vm->local_lock());
      if(!vm->wakeup()) return (Thread*)Qnil;
    }

    return this;
  }

#define NANOSECONDS 1000000000
  Object* Thread::sleep_now(STATE, Object* duration) {
    struct timespec ts;
    struct timeval tv;

    gettimeofday(&tv, 0);
    ts.tv_sec =  tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;

    if(Fixnum* fix = try_as<Fixnum>(duration)) {
      ts.tv_sec += fix->to_native();
    } else if(Float* flt = try_as<Float>(duration)) {
      uint64_t nano = (uint64_t)(flt->val * NANOSECONDS);
      ts.tv_sec +=  (time_t)(nano / NANOSECONDS);
      ts.tv_nsec +=   (long)(nano % NANOSECONDS);
    } else if(duration->nil_p()) {
      ts.tv_sec = 0;
      ts.tv_nsec = 0;
    } else {
      return Primitives::failure();
    }

    thread::Condition cond;
    WaitingOnCondition waiter(cond);

    thread::Mutex& mutex = state->local_lock();
    mutex.lock();
    state->install_waiter(waiter);
    {
      GlobalLock::UnlockGuard x(state->global_lock());
      cond.wait_until(mutex, &ts);
    }
    mutex.unlock();

    return Qnil;
  }


/* Interface */

  void Thread::boot_task(STATE) {
    Task* task = Task::create(state);
    this->task(state, task);
  }

  void Thread::set_top(STATE, Object* val) {
    task_->active()->set_top(val);
  }

  void Thread::sleep_for(STATE, Channel* chan) {
    channel(state, chan);
    sleep(state, Qtrue);
  }

}
