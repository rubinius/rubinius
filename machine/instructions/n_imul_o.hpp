#include "instructions.hpp"
#include "class/bignum.hpp"

#include <climits>

namespace rubinius {
  namespace instructions {
    inline void n_imul_o(STATE, CF, R0, R1, R2) {
      long long int r = STRIP_FIXNUM_TAG(REG(r1)) * STRIP_FIXNUM_TAG(REG(r2));

      if(r > LLONG_MAX || r < LLONG_MIN) {
        RVAL(r0) = Bignum::from(state, r);
      } else {
        RVAL(r0) = APPLY_FIXNUM_TAG(r);
      }
    }
  }
}
