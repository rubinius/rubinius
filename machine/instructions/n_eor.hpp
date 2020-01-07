#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_eor(STATE, CF, R0, R1, R2) {
      RVAL(r0) = as<Bignum>(RVAL(r1))->bit_or(state, as<Bignum>(RVAL(r2)));
    }
  }
}
