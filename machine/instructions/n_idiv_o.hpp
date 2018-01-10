#include "instructions.hpp"
#include "class/bignum.hpp"

#include <climits>

namespace rubinius {
  namespace instructions {
    inline void n_idiv_o(STATE, CF, R0, R1, R2) {
      intptr_t r = STRIP_FIXNUM_TAG(REG(r2));

      if(r == 0) {
        Exception::raise_zero_division_error(state, "divided by 0");
      }

      r = STRIP_FIXNUM_TAG(REG(r1)) / r;

      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        RVAL(r0) = Bignum::from(state, r);
      } else {
        RVAL(r0) = APPLY_FIXNUM_TAG(r);
      }
    }
  }
}
