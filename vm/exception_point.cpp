#include "exception_point.hpp"
#include "builtin/nativemethod.hpp"

namespace rubinius {
  ExceptionPoint::ExceptionPoint(NativeMethodEnvironment* env)
    : jumped_to_(false)
    , previous_(env->current_ep())
  {
    env->set_current_ep(this);
  }

  void ExceptionPoint::return_to(NativeMethodEnvironment* env) {
    jumped_to_ = true;
    env->set_current_ep(this);
    _longjmp(__jump_buffer, 1);

    // If control reaches here, longjmp failed, i.e. disaster.
    abort();
  }

  void ExceptionPoint::pop(NativeMethodEnvironment* env) {
    env->set_current_ep(previous_);
  }
}
