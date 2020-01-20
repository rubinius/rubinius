#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_ecmp(STATE, CF, R0, R1, R2) {
      Bignum* a = as<Bignum>(RVAL(r1));
      Bignum* b = as<Bignum>(RVAL(r2));

      if(CBOOL(a->equal(state, b))) {
        RVAL(r0) = APPLY_FIXNUM_TAG(0);
      } else if(CBOOL(a->lt(state, b))) {
        RVAL(r0) = FIXNUM_NEG_ONE;
      } else {
        RVAL(r0) = APPLY_FIXNUM_TAG(1);
      }
    }
  }
}
