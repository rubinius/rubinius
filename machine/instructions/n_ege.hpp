#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_ege(STATE, CF, R0, R1, R2) {
      REG(r0) = CBOOL(as<Bignum>(RVAL(r1))->ge(state, as<Bignum>(RVAL(r2))));
    }
  }
}
