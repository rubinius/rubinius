/* The Channel class, provides a simple, Ruby thread safe communication
 * mechanism. */

#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/channel.hpp"
#include "builtin/thread.hpp"
#include "builtin/list.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/io.hpp"
#include "builtin/contexts.hpp"

#include "message.hpp"
#include "event.hpp"

namespace rubinius {
  Channel* Channel::create(STATE) {
    Channel* chan = (Channel*)state->new_object(G(channel));
    chan->waiting(state, List::create(state));

    return chan;
  }

  /** @todo Remove the event too? Should not affect code, but no need for it either. --rue */
  void Channel::cancel_waiter(STATE, const Thread* waiter) {
    waiting_->remove(state, waiter);
  }

  Object* Channel::send(STATE, Object* val) {
    if(!waiting_->empty_p()) {
      Thread* thr = as<Thread>(waiting_->shift(state));
      thr->set_top(state, val);
      thr->wakeup(state);
      return Qnil;
    }

    if(List* lst = try_as<List>(value_)) {
      lst->append(state, val);
    } else {
      List* lst = List::create(state);
      lst->append(state, val);

      value(state, lst);
    }

    return Qnil;
  }

  /**
   * @todo   The list management is iffy. Should probably just
   *          always assume it is a list. --rue
   */
  ExecuteStatus Channel::receive_prim(STATE, Executable* exec, Task* task, Message& msg) {
    Thread* current = state->globals.current_thread.get();

    task->active()->clear_stack(msg.stack);

    // @todo  check arity
    if(!value_->nil_p()) {
      List* list = as<List>(value_);

      task->push(list->shift(state));

      if(list->size() == 0) {
        value(state, Qnil);
      }

      return cExecuteContinue;
    }

    /* Saves space plus if thread woken forcibly, it gets the Qfalse. */
    task->push(Qfalse);

    current->sleep_for(state, this);
    waiting_->append(state, current);

    state->check_events();

    /* This sets the Task to continue from the next
     * opcode when it eventually reactivates. Its
     * stack will then have either the real received
     * value or the default Qfalse if the thread
     * was forced to stop waiting for us.
     */
    return cExecuteRestart;
  }

  Object* Channel::receive(STATE) {
    if(!value_->nil_p()) {
      List* list = as<List>(value_);

      state->return_value(list->shift(state));

      if(list->size() == 0) {
        value(state, Qnil);
      }

      return Qnil;
    }

    /* We push nil on the stack to reserve a place to put the result. */
    state->return_value(Qfalse);

    G(current_thread)->sleep_for(state, this);
    waiting_->append(state, G(current_thread));

    state->run_best_thread();

    return Qnil;
  }

  bool Channel::has_readers_p() {
    return !waiting_->empty_p();
  }

  class SendToChannel : public ObjectCallback {
  public:
    TypedRoot<Channel*> chan;

    SendToChannel(STATE, Channel* chan) : ObjectCallback(state), chan(state, chan) { }

    virtual Object* object() {
      return chan.get();
    }

    virtual void call(Object* obj) {
      chan->send(state, obj);
    }
  };

  Object* Channel::send_on_signal(STATE, Channel* chan, Fixnum* signal) {
    SendToChannel* cb = new SendToChannel(state, chan);
    event::Signal* sig = new event::Signal(state, cb, signal->to_native());
    state->signal_events->start(sig);
    return signal;
  }

  Object* Channel::send_on_readable(STATE, Channel* chan, IO* io,
      Object* maybe_buffer, Fixnum* bytes) {

    SendToChannel* cb = new SendToChannel(state, chan);
    event::Read* sig = new event::Read(state, cb, io->to_fd());
    sig->into_buffer(maybe_buffer, bytes->to_native());

    state->events->start(sig);
    return io;
  }

  Object* Channel::send_in_microseconds(STATE, Channel* chan, Integer* useconds, Object* tag) {
    double seconds = useconds->to_native() / 1000000.0;

    return send_in_seconds(state, chan, seconds, tag);
  }

  Object* Channel::send_in_seconds(STATE, Channel* chan, Float* seconds, Object* tag) {
    double secs = seconds->to_double(state);
    return send_in_seconds(state, chan, secs, tag);
  }

  Object* Channel::send_in_seconds(STATE, Channel* chan, double seconds, Object* tag) {
    SendToChannel* cb = new SendToChannel(state, chan);
    event::Timer* sig = new event::Timer(state, cb, seconds, tag);
    state->events->start(sig);
    return Qnil;
  }

  Object* Channel::send_on_stopped(VM* state, Channel* channel, Fixnum* pid, Fixnum* flags)
  {
    event::Child::add(state,
                      new SendToChannel(state, channel),
                      static_cast<pid_t>(pid->to_int()),
                      flags->to_int() );

    /* @todo Figure out the cancellation by id method. */
    return Qnil;
  }



/* ChannelCallback */

  ChannelCallback::ChannelCallback(STATE, Channel* chan) : ObjectCallback(state) {
    channel.set(chan, &GO(roots));
  }

  void ChannelCallback::call(Object* obj) {
    channel->send(state, obj);
  }
}
