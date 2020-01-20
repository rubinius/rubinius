#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_icmp(CF, R0, R1, R2) {
      intptr_t a = STRIP_FIXNUM_TAG(REG(r1));
      intptr_t b = STRIP_FIXNUM_TAG(REG(r2));

      if(a == b) {
        RVAL(r0) = APPLY_FIXNUM_TAG(0);
      } else if(a < b) {
        RVAL(r0) = FIXNUM_NEG_ONE;
      } else {
        RVAL(r0) = APPLY_FIXNUM_TAG(1);
      }
    }
  }
}
