#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_false(CF, R0) {
      RVAL(r0) = cFalse;
    }
  }
}
