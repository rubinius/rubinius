#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_eand(STATE, CF, R0, R1, R2) {
      RVAL(r0) = as<Bignum>(RVAL(r1))->bit_and(state, as<Bignum>(RVAL(r2)));
    }
  }
}
