#include "instructions.hpp"

#include <cmath>

namespace rubinius {
  namespace instructions {
    inline void n_isize(CF, R0, R1) {
      intptr_t r = STRIP_FIXNUM_TAG(RVAL(r1));

      RVAL(r0) = APPLY_FIXNUM_TAG(static_cast<intptr_t>(ceil(log2(r < 0 ? -r : r+1))));
    }
  }
}
