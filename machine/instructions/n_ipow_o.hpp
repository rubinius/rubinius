#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_ipow_o(STATE, CF, R0, R1, R2) {
      RVAL(r0) = as<Fixnum>(RVAL(r1))->pow(state, as<Fixnum>(RVAL(r2)));
    }
  }
}
