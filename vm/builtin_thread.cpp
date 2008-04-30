#include "builtin.hpp"

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
    SET(thr, task, Task::create(state));
    SET(thr, priority, Object::i2n(2));
    return thr;
  }
}
