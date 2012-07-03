/* The Channel class, provides a simple, Ruby thread safe communication
 * mechanism. */

#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/object.hpp"
#include "builtin/channel.hpp"
#include "builtin/thread.hpp"
#include "builtin/list.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/io.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"

#include "on_stack.hpp"

#include "ontology.hpp"

#include <sys/time.h>

namespace rubinius {

  void Channel::init(STATE) {
    GO(channel).set(ontology::new_class(state, "Channel",
                      G(object), G(rubinius)));
    G(channel)->set_object_type(state, Channel::type);
  }

  Channel* Channel::create(STATE) {
    Channel* chan = state->vm()->new_object_mature<Channel>(G(channel));
    chan->waiters_ = 0;
    chan->semaphore_count_ = 0;

    // Using placement new to call the constructor of condition_
    new(&chan->condition_) utilities::thread::Condition();
    new(&chan->mutex_) utilities::thread::Mutex();

    chan->value(state, List::create(state));

    return chan;
  }

  Channel* Channel::create_primed(STATE) {
    Channel* chan = state->vm()->new_object_mature<Channel>(G(channel));
    chan->waiters_ = 0;
    chan->semaphore_count_ = 1;

    // Using placement new to call the constructor of condition_
    new(&chan->condition_) utilities::thread::Condition();
    new(&chan->mutex_) utilities::thread::Mutex();

    chan->value(state, List::create(state));

    return chan;
  }

  /** @todo Remove the event too? Should not affect code, but no need for it either. --rue */
  void Channel::cancel_waiter(STATE, const Thread* waiter) {
    // waiting_->remove(state, waiter);
  }

  Object* Channel::send(STATE, GCToken gct, Object* val) {
    Channel* self = this;

    OnStack<2> os(state, val, self);

    GCLockGuard lg(state, gct, mutex_);

    if(val->nil_p()) {
      self->semaphore_count_++;
    } else {
      if(self->semaphore_count_ > 0) {
        for(int i = 0; i < self->semaphore_count_; i++) {
          self->value_->append(state, cNil);
        }
        self->semaphore_count_ = 0;
      }

      self->value_->append(state, val);
    }

    if(self->waiters_ > 0) {
      self->condition_.signal();
    }

    return cNil;
  }

  Object* Channel::try_receive(STATE, GCToken gct) {
    Channel* self = this;
    OnStack<1> os(state, self);

    GCLockGuard lg(state, gct, mutex_);

    if(self->semaphore_count_ > 0) {
      self->semaphore_count_--;
      return cNil;
    }

    if(self->value_->empty_p()) return cNil;
    return self->value_->shift(state);
  }

  Object* Channel::receive(STATE, GCToken gct, CallFrame* call_frame) {
    return receive_timeout(state, gct, cNil, call_frame);
  }

#define NANOSECONDS 1000000000
  Object* Channel::receive_timeout(STATE, GCToken gct, Object* duration, CallFrame* call_frame) {
    // Passing control away means that the GC might run. So we need
    // to stash this into a root, and read it back out again after
    // control is returned.
    //
    // DO NOT USE this AFTER wait().

    // We have to do this because we can't pass this to OnStack, since C++
    // won't let us reassign it.

    Channel* self = this;
    OnStack<2> os(state, self, duration);

    GCLockGuard lg(state, gct, mutex_);

    if(self->semaphore_count_ > 0) {
      self->semaphore_count_--;
      return cNil;
    }

    if(!self->value_->empty_p()) return self->value_->shift(state);

    // Otherwise, we need to wait for a value.
    struct timespec ts = {0,0};
    bool use_timed_wait = true;

    if(Fixnum* fix = try_as<Fixnum>(duration)) {
      ts.tv_sec = fix->to_native();
    } else if(Float* flt = try_as<Float>(duration)) {
      uint64_t nano = (uint64_t)(flt->val * NANOSECONDS);
      ts.tv_sec  =  (time_t)(nano / NANOSECONDS);
      ts.tv_nsec =    (long)(nano % NANOSECONDS);
    } else if(duration->nil_p()) {
      use_timed_wait = false;
    } else {
      return Primitives::failure();
    }

    // We pin this so we can pass condition_ out without worrying about
    // us moving it.
    if(!self->pin()) {
      rubinius::bug("unable to pin Channel");
    }

    struct timeval tv = {0,0};
    if(use_timed_wait) {
      gettimeofday(&tv, 0);
      uint64_t nano = ts.tv_nsec + tv.tv_usec * 1000;
      ts.tv_sec  += tv.tv_sec + nano / NANOSECONDS;
      ts.tv_nsec  = nano % NANOSECONDS;
    }

    // We lock to manipulate the wait condition on the VM* so that
    // we can sync up properly with another thread trying to wake us
    // up right as we're trying to go to sleep.
    state->lock(gct);

    if(!state->check_async(call_frame)) {
      state->unlock();
      return NULL;
    }

    state->vm()->wait_on_channel(self);

    state->unlock();

    self->waiters_++;

    for(;;) {
      {
        GCIndependent gc_guard(state, call_frame);

        if(use_timed_wait) {
          if(self->condition_.wait_until(self->mutex_, &ts) == utilities::thread::cTimedOut) break;
        } else {
          self->condition_.wait(self->mutex_);
        }
      }

      // or there are values available.
      if(self->semaphore_count_ > 0 || !self->value()->empty_p()) break;
    }

    state->vm()->clear_waiter();
    state->vm()->thread->sleep(state, cFalse);

    self->unpin();
    self->waiters_--;

    if(!state->check_async(call_frame)) return NULL;

    if(self->semaphore_count_ > 0) {
      self->semaphore_count_--;
      return cNil;
    }

    // We were awoken, but there is no value to use. Return false.
    if(self->value()->empty_p()) return cFalse;

    return self->value()->shift(state);
  }


  bool Channel::has_readers_p() {
    return waiters_ > 0;
  }

  class SendToChannel : public ObjectCallback {
  public:
    TypedRoot<Channel*> chan;

    SendToChannel(STATE, Channel* chan) : ObjectCallback(state), chan(state, chan) { }

    virtual Object* object() {
      return chan.get();
    }

    virtual void call(Object* obj) {
      GCTokenImpl gct;
      chan->send(state, gct, obj);
    }
  };

/* ChannelCallback */

  ChannelCallback::ChannelCallback(STATE, Channel* chan) : ObjectCallback(state) {
    channel.set(chan, &GO(roots));
  }

  void ChannelCallback::call(Object* obj) {
    GCTokenImpl gct;
    channel->send(state, gct, obj);
  }
}
