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

#include <chrono>

namespace rubinius {

  void Channel::bootstrap(STATE) {
    GO(channel).set(state->memory()->new_class<Class, Channel>(
          state, G(rubinius), "Channel"));
  }

  Channel* Channel::create(STATE) {
    Channel* chan = state->memory()->new_object_pinned<Channel>(state, G(channel));

    // Other values are set in ::initialize().
    chan->value(state, List::create(state));

    return chan;
  }

  Object* Channel::send(STATE, Object* val) {
    std::unique_lock<std::mutex> guard(_mutex_);

    bool was_empty = value()->empty_p();

    value()->append(state, val);

    if(was_empty) {
      _condition_.notify_all();
    }

    return cNil;
  }

  Object* Channel::try_receive(STATE) {
    std::unique_lock<std::mutex> guard(_mutex_);

    if(value()->empty_p()) return cNil;

    return value()->shift(state);
  }

  Object* Channel::receive(STATE) {
    return receive_timeout(state, cNil);
  }

  Object* Channel::receive_timeout(STATE, Object* duration) {
    std::unique_lock<std::mutex> lock(_mutex_);

    if(!value()->empty_p()) {
      return value()->shift(state);
    }

    Channel* self = this;
    OnStack<1> os(state, self);

    double seconds = 0;

    if(Fixnum* fix = try_as<Fixnum>(duration)) {
      seconds = fix->to_native();
    } else if(Float* flt = try_as<Float>(duration)) {
      seconds = flt->value();
    } else if(duration->nil_p()) {
      seconds = 0.01;
    } else {
      return Primitives::failure();
    }

    {
      UnmanagedPhase guard(state);

      std::chrono::duration<double> pause(seconds);

      while(self->value()->empty_p() && !state->thread_interrupted_p()) {
        auto status = self->_condition_.wait_for(lock, pause);

        if(!self->value()->empty_p()) {
          return self->value()->shift(state);
        }

        if(status == std::cv_status::timeout && !duration->nil_p()) {
          // We were woken because duration elapsed.
          return cNil;
        }
      }
    }

    // Unreachable, but satisfy the compiler.
    return cNil;
  }
}
