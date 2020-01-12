#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_edivmod(STATE, CF, R0, R1, R2) {
      Integer* m = Fixnum::from(0);
      Integer* q = as<Bignum>(RVAL(r1))->divide(state, as<Bignum>(RVAL(r2)), &m);

      RVAL(r1) = q;
      RVAL(r2) = m;
    }
  }
}
