#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_exor(STATE, CF, R0, R1, R2) {
      RVAL(r0) = as<Bignum>(RVAL(r1))->bit_xor(state, as<Bignum>(RVAL(r2)));
    }
  }
}
