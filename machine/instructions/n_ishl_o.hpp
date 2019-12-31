#include "instructions.hpp"
#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_ishl_o(STATE, CF, R0, R1, R2) {
      intptr_t x = STRIP_FIXNUM_TAG(REG(r1));
      intptr_t y = STRIP_FIXNUM_TAG(REG(r2));

      if((x > 0 && (y >= FIXNUM_MAX_WIDTH || x > (FIXNUM_MAX >> y)))
        || (x < 0 && (y >= FIXNUM_MIN_WIDTH || x < (FIXNUM_MIN >> y)))) {
        RVAL(r0) = Bignum::from(state, x)->left_shift(state, Fixnum::from(y));
      } else {
        intptr_t v = x << y;

        RVAL(r0) = APPLY_FIXNUM_TAG(v);
      }
    }
  }
}
