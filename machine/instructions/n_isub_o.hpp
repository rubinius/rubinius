#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_isub_o(STATE, CF, R0, R1, R2) {
      intptr_t r = STRIP_FIXNUM_TAG(REG(r1)) - STRIP_FIXNUM_TAG(REG(r2));

      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        RVAL(r0) = Bignum::from(state, r);
      } else {
        RVAL(r0) = APPLY_FIXNUM_TAG(r);
      }
    }
  }
}
