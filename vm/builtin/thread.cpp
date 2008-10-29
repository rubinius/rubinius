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
    Tuple* tup = Tuple::from(state, 3,
                             List::create(state),
                             List::create(state),
                             List::create(state) );

    GO(scheduled_threads).set(tup);

    GO(thread).set(state->new_class("Thread", G(object), Thread::fields));
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
      replacement->copy_from(state, scheduled, Fixnum::from(0), Fixnum::from(0));

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

    if(!channel()->nil_p()) {
      channel()->cancel_waiter(state, this);
    }

    channel(state, (Channel*)Qnil);

    state->dequeue_thread(this);
    state->find_and_activate_thread();

    return this;
  }

  Object* Thread::pass(STATE) {
    /* Stash the Task in case it has changed. @todo Overcautious? --rue */
    this->task(state, state->globals.current_task.get());

    /* Delay queuing until another one is found to allow lower priority. */
    if(state->find_and_activate_thread()) {
      state->queue_thread(this);
    }

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
      return reinterpret_cast<Thread*>(kPrimitiveFailed);
    }

    sleep(state, Qfalse);

    /** @todo   This is possibly unnecessary except for raise() and exited(). --rue */
    if(!channel()->nil_p()) {
      channel()->cancel_waiter(state, this);
    }

    channel(state, (Channel*)Qnil);

    state->queue_thread(this);

    return this;
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
