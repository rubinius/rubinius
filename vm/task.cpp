#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include "task.hpp"

namespace rubinius {
  void Task::init(STATE) {
    Tuple* tup = Tuple::from(state, 7, List::create(state), List::create(state),
        List::create(state), List::create(state), List::create(state),
        List::create(state), List::create(state));

    G(scheduled_threads) = tup;

    Class* cls = state->new_class("Task", Task::fields);
    cls->set_const(state, "ScheduledThreads", tup);

    G(channel) = state->new_class("Channel", Channel::fields);
    G(channel)->set_object_type(Channel::type);

    G(thread) =  state->new_class("Thread",  Thread::fields);
    G(thread)->set_object_type(Thread::type);
  }

  void Channel::send(OBJECT obj) {
  }
}
