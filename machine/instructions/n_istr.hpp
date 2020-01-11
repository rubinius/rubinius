#include "instructions.hpp"

#include "class/fixnum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_istr(STATE, CF, R0, R1, R2) {
      RVAL(r0) = as<Fixnum>(RVAL(r1))->to_s(state, as<Fixnum>(RVAL(r2)));
    }
  }
}
