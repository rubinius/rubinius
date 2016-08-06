#include "interpreter/instructions.hpp"

#include "helpers.hpp"

namespace rubinius {
  namespace instructions {
    inline void yield_debugger(STATE) {
      Helpers::yield_debugger(state, cNil);
    }
  }
}
