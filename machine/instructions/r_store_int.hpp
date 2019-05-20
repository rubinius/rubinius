#include "instructions.hpp"
#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_store_int(STATE, CF, R0, R1) {
      intptr_t r = REG(r1);

      if(r > FIXNUM_MAX || r < FIXNUM_MIN) {
        RVAL(r0) = Bignum::from(state, r);
      } else {
        RVAL(r0) = APPLY_FIXNUM_TAG(r);
      }
    }
  }
}
