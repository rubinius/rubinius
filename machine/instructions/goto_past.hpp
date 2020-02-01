#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void goto_past(STATE, CallFrame* call_frame) {
      state->checkpoint(state);
    }
  }
}
