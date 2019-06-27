#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_true(CF, R0) {
      RVAL(r0) = cTrue;
    }
  }
}
