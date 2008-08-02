#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"
#include "builtin/list.hpp"

#include "objectmemory.hpp"

namespace rubinius {
  void Thread::init(STATE) {
    Tuple* tup = Tuple::from(state, 7, List::create(state), List::create(state),
        List::create(state), List::create(state), List::create(state),
        List::create(state), List::create(state));

    GO(scheduled_threads).set(tup);

    GO(thread).set(state->new_class("Thread",  Thread::fields));
    G(thread)->set_object_type(Thread::type);
    
    G(thread)->set_const(state, "ScheduledThreads", tup);
  }

  Thread* Thread::create(STATE) {
    Thread* thr = (Thread*)state->new_object(G(thread));
    SET(thr, priority, Object::i2n(2));
    return thr;
  }

  Thread* Thread::current(STATE) {
    return state->current_thread();
  }

  void Thread::boot_task(STATE) {
    Task* task = Task::create(state);
    SET(this, task, task);
  }

  void Thread::set_top(STATE, OBJECT val) {
    task->stack->put(state, task->sp, val);
  }
  
  void Thread::sleep_for(STATE, Channel* chan) {
    channel = chan;
    set_ivar(state, state->symbol("@sleep"), Qtrue);
  }
}
