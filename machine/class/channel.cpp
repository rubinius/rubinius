#include "object_utils.hpp"
#include "on_stack.hpp"
#include "memory.hpp"
#include "primitives.hpp"
#include "thread_phase.hpp"

#include "class/class.hpp"
#include "class/channel.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/io.hpp"
#include "class/list.hpp"
#include "class/object.hpp"
#include "class/thread.hpp"

#include <sys/time.h>

namespace rubinius {

  void Channel::bootstrap(STATE) {
    GO(channel).set(state->memory()->new_class<Class, Channel>(
          state, G(rubinius), "Channel"));
  }

  Channel* Channel::create(STATE) {
    Channel* chan = state->memory()->new_object_pinned<Channel>(state, G(channel));

    chan->waiters(0);
    chan->semaphore_count(0);

    // Using placement new to call the constructor of _condition_ and _mutex_
    new(&chan->_condition_) std::condition_variable;
    new(&chan->_mutex_) std::mutex;

    chan->value(state, List::create(state));

    return chan;
  }

  Object* Channel::send(STATE, Object* val) {
    std::lock_guard<std::mutex> guard(_mutex_);

    if(val->nil_p()) {
      inc_semaphore_count();
    } else {
      if(semaphore_count() > 0) {
        for(int i = 0; i < semaphore_count(); i++) {
          value()->append(state, cNil);
        }
        semaphore_count(0);
      }

      value()->append(state, val);
    }

    if(waiters() > 0) {
      _condition_.notify_one();
    }

    return cNil;
  }

  Object* Channel::try_receive(STATE) {
    std::lock_guard<std::mutex> guard(_mutex_);

    if(semaphore_count() > 0) {
      dec_semaphore_count();
      return cNil;
    }

    if(value()->empty_p()) return cNil;

    return value()->shift(state);
  }

  Object* Channel::receive(STATE) {
    return receive_timeout(state, cNil);
  }

  Object* Channel::receive_timeout(STATE, Object* duration) {
    {
      std::lock_guard<std::mutex> guard(_mutex_);

      if(semaphore_count() > 0) {
        dec_semaphore_count();
        return cNil;
      }

      if(!value()->empty_p()) {
        return value()->shift(state);
      }
    }

    Channel* self = this;
    OnStack<1> os(state, self);

    double seconds = 0;

    if(Fixnum* fix = try_as<Fixnum>(duration)) {
      seconds = fix->to_native();
    } else if(Float* flt = try_as<Float>(duration)) {
      seconds = flt->value();
    } else if(duration->nil_p()) {
      seconds = -1;
    } else {
      return Primitives::failure();
    }

    if(state->thread_interrupted_p(state)) {
      return NULL;
    }

    state->wait_on_channel(self);
    self->inc_waiters();

    bool exception = false;

    while(true) {
      {
        UnmanagedPhase unmanaged(state);

        if(seconds >= 0) {
          std::chrono::duration<double> pause(seconds);

          std::unique_lock<std::mutex> lk(self->_mutex_);

          if(self->_condition_.wait_for(lk, pause) == std::cv_status::timeout) {
            break;
          }
        } else {
          std::unique_lock<std::mutex> lk(self->_mutex_);
          self->_condition_.wait(lk);
        }
      }

      if(self->semaphore_count() > 0 || !self->value()->empty_p()) {
        break;
      }

      if(state->thread_interrupted_p(state)) {
        exception = true;
        break;
      }
    }

    state->clear_waiter();
    state->thread()->sleep(state, cFalse);

    self->_waiters_--;

    if(exception) return nullptr;

    if(self->semaphore_count() > 0) {
      self->dec_semaphore_count();
      return cNil;
    }

    // We were awoken, but there is no value to use. Return false.
    if(self->value()->empty_p()) return cFalse;

    return self->value()->shift(state);
  }
}
