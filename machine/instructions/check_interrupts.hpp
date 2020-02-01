#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void check_interrupts(STATE) {
      state->checkpoint(state);
    }
  }
}
