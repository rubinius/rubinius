#include "instructions.hpp"

#include "class/unwind_state.hpp"

namespace rubinius {
  namespace instructions {
    inline void clear_exception(STATE) {
      state->unwind_state()->clear_raise();
    }
  }
}
