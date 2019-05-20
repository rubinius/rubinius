#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_stack(CF, R0) {
      RVAL(r0) = stack_top();
    }
  }
}
