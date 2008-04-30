#include "builtin.hpp"

namespace rubinius {
  Channel* Channel::create(STATE) {
    Channel* chan = (Channel*)state->new_object(G(channel));
    SET(chan, waiting, List::create(state));

    return chan;
  }

  void Channel::send(STATE, OBJECT val) {
    if(value->nil_p()) {
      List* lst = List::create(state);
      lst->append(state, val);

      value = lst;
    }
  }

  void Channel::receive(STATE) {
    if(!value->nil_p()) {
      OBJECT val = as<List>(value)->shift(state);
      state->return_value(val);
      return;
    }

    /* We push nil on the stack to reserve a place to put the result. */
    state->return_value(Qfalse);

    G(current_thread)->sleep_for(state, this);
    waiting->append(state, G(current_thread));
    state->run_best_thread();
  }

  bool Channel::has_readers_p() {
    return false;
  }

  void Thread::sleep_for(STATE, Channel* chan) {

  }
}
