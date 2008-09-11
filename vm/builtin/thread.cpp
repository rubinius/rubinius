#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/contexts.hpp"

#include "objectmemory.hpp"
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

  Thread* Thread::create(STATE) {
    Thread* thr = (Thread*)state->new_object(G(thread));
    thr->priority(state, Fixnum::from(2));

    thr->boot_task(state);

    return thr;
  }

  Thread* Thread::current(STATE) {
    return state->current_thread();
  }

  void Thread::boot_task(STATE) {
    Task* task = Task::create(state);
    this->task(state, task);
  }

  Object* Thread::pass(STATE) {
    Thread* current = Thread::current(state);

    state->queue_thread(current);
    state->run_best_thread();

    return Qnil;
  }

  Thread* Thread::run(STATE) {
    state->activate_thread(this);
    return this;
  }

  void Thread::set_top(STATE, OBJECT val) {
    task_->active()->set_top(val);
  }

  void Thread::sleep_for(STATE, Channel* chan) {
    channel(state, chan);
    set_ivar(state, state->symbol("@sleep"), Qtrue);
  }

  Thread* Thread::wakeup(STATE) {
    state->queue_thread(this);
    return this;
  }

}
