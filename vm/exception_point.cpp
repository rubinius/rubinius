#include <setjmp.h>

#include "config.h"
#include "exception_point.hpp"
#include "builtin/native_method.hpp"
#include "windows_compat.h"

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
    long_jump(__jump_buffer, 1);

    // If control reaches here, longjmp failed, i.e. disaster.
    rubinius::bug("ExceptionPoint return_to failed");
  }

  void ExceptionPoint::pop(NativeMethodEnvironment* env) {
    env->set_current_ep(previous_);
  }
}
