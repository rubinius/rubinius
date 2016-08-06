#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void clear_exception(STATE) {
      state->vm()->thread_state()->clear_raise();
    }
  }
}
