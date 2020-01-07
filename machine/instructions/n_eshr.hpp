#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_eshr(STATE, CF, R0, R1, R2) {
      RVAL(r0) = as<Bignum>(RVAL(r1))->right_shift(state, Fixnum::from(REG(r2)));
    }
  }
}
