#include "instructions.hpp"

#include <cmath>

namespace rubinius {
  namespace instructions {
    inline void n_isize(CF, R0, R1) {
      RVAL(r0) = APPLY_FIXNUM_TAG(sizeof(intptr_t));
    }
  }
}
