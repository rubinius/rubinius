#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_eflt(STATE, CF, R0, R1) {
      RFLT(r0) = as<Bignum>(RVAL(r1))->to_double(state);
    }
  }
}
