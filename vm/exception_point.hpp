#ifndef RBX_EXCEPTION_POINT_HPP
#define RBX_EXCEPTION_POINT_HPP

#include <setjmp.h>

#include "windows_compat.h"

namespace rubinius {
  class NativeMethodEnvironment;

  class ExceptionPoint {
    bool jumped_to_;
    ExceptionPoint* previous_;

  public:
    jmp_buf __jump_buffer;
    const char* file;
    int line;

  public:
    ExceptionPoint(NativeMethodEnvironment* env);

    bool jumped_to() const {
      return jumped_to_;
    }

    void reset() {
      jumped_to_ = false;
    }

    void return_to(NativeMethodEnvironment* env);

    void unwind_to_previous(NativeMethodEnvironment* env) {
      previous_->return_to(env);
    }

    void pop(NativeMethodEnvironment* env);
  };
}

#define PLACE_EXCEPTION_POINT(ep) ep.file = __FILE__; ep.line = __LINE__; set_jump(ep.__jump_buffer)

#endif
