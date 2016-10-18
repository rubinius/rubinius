#include "call_frame.hpp"
#include "helpers.hpp"
#include "memory.hpp"
#include "park.hpp"
#include "signal.hpp"
#include "state.hpp"
#include "vm.hpp"

#include "class/exception.hpp"
#include "class/location.hpp"
#include "class/array.hpp"
#include "class/fiber.hpp"

namespace rubinius {
  void State::raise_stack_error(STATE) {
    Class* stack_error = globals().stack_error.get();
    Exception* exc = memory()->new_object<Exception>(this, stack_error);
    exc->locations(this, Location::from_call_stack(this));
    vm_->thread_state()->raise_exception(exc);
  }

  Object* State::park(STATE) {
    return vm_->park_->park(this);
  }

  Object* State::park_timed(STATE, struct timespec* ts) {
    return vm_->park_->park_timed(this, ts);
  }
}
