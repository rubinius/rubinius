#include "instructions.hpp"
#include "class/bignum.hpp"

#include <cstdint>

namespace rubinius {
  namespace instructions {
    inline void n_imul_o(STATE, CF, R0, R1, R2) {
// Adapted from the logic in 1.9
// tests if N*N would overflow
#define SQRT_LONG_MAX ((intptr_t)1<<((sizeof(intptr_t)*CHAR_BIT-1)/2))
#define FIT_SQRT(n) (((n)<SQRT_LONG_MAX)&&((n)>-SQRT_LONG_MAX))
#define NO_OVERFLOW_MUL(a,b) (FIT_SQRT(a)&&FIT_SQRT(b))
#define OVERFLOW_MUL(a,b) (!(NO_OVERFLOW_MUL(a,b)))
      intptr_t a = STRIP_FIXNUM_TAG(REG(r1));
      intptr_t b = STRIP_FIXNUM_TAG(REG(r2));

      if(NO_OVERFLOW_MUL(a, b)) {
        RVAL(r0) = APPLY_FIXNUM_TAG(a * b);
      } else {
        RVAL(r0) = Bignum::from(state, a)->mul(state, reinterpret_cast<Fixnum*>(REG(r2)));
      }
    }
  }
}
