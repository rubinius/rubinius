#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/contexts.hpp"
#include "builtin/channel.hpp"

#include "objectmemory.hpp"
#include "message.hpp"

#include "vm/object_utils.hpp"
#include "vm.hpp"

namespace rubinius {
  void Thread::init(STATE) {
    Tuple* tup = Tuple::from(state, 7, List::create(state), List::create(state),
        List::create(state), List::create(state), List::create(state),
        List::create(state), List::create(state));

    GO(scheduled_threads).set(tup);

    GO(thread).set(state->new_class("Thread", G(object), Thread::fields));
    G(thread)->set_object_type(state, Thread::type);

    G(thread)->set_const(state, "ScheduledThreads", tup);
  }


/* Primitives */

  Thread* Thread::create(STATE) {
    Thread* thr = (Thread*)state->new_object(G(thread));

    thr->alive(state, Qtrue);
    thr->channel(state, reinterpret_cast<Channel*>(Qnil));
    thr->priority(state, Fixnum::from(2));
    thr->queued(state, Qfalse);
    thr->sleep(state, Qtrue);

    thr->boot_task(state);

    state->interrupts.use_preempt = true;
    state->interrupts.enable_preempt = true;

    return thr;
  }

  Thread* Thread::current(STATE) {
    return state->globals.current_thread.get();
  }

  /** @todo   Add voluntary/involuntary? --rue */
  Object* Thread::exited(STATE) {
    alive(state, Qfalse);
    channel(state, (Channel*)Qnil);

    state->dequeue_thread(this);
    state->find_and_activate_thread();

    return this;
  }

  /** @todo   Should avoid running this thread (e.g. some lower-priority.) */
  Object* Thread::pass(STATE) {

    state->queue_thread(Thread::current(state));
    state->find_and_activate_thread();

    return Qnil;
  }

  Object* Thread::raise(STATE, Exception* error) {
    wakeup(state);

    MethodContext* ctx = task_->active();
    ctx->reference(state);
    error->context(state, ctx);

    return task_->raise(state, error);
  }

  Thread* Thread::wakeup(STATE) {
    if(alive() == Qfalse) {
      return Thread::current(state);
    }

    woken(state);

    state->queue_thread(this);

    return this;
  }

  void Thread::woken(STATE) {
    sleep(state, Qfalse);
    /* Clearing the channel is OK because a sleep check by the VM. */
    channel(state, (Channel*)Qnil);
  }

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
