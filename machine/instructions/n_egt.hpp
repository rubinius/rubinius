#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_egt(STATE, CF, R0, R1, R2) {
      REG(r0) = CBOOL(as<Bignum>(RVAL(r1))->gt(state, as<Bignum>(RVAL(r2))));
    }
  }
}
