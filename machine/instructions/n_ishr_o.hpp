#include "instructions.hpp"
#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_ishr_o(STATE, CF, R0, R1, R2) {
      intptr_t x = STRIP_FIXNUM_TAG(REG(r1));
      intptr_t y = STRIP_FIXNUM_TAG(REG(r2));

      if(x > 0 && y > FIXNUM_MAX_WIDTH) {
        RVAL(r0) = APPLY_FIXNUM_TAG(0);
      } else if(x < 0 && y > FIXNUM_MIN_WIDTH) {
        RVAL(r0) = FIXNUM_NEG_ONE;
      } else {
        RVAL(r0) = APPLY_FIXNUM_TAG(x >> y);
      }
    }
  }
}
