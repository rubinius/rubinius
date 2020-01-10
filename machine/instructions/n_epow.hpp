#include "instructions.hpp"

#include "class/bignum.hpp"
#include "class/fixnum.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_epow(STATE, CF, R0, R1, R2) {
      if(Fixnum* fix = try_as<Fixnum>(Bignum::normalize(state, as<Bignum>(RVAL(r1))))) {
        RVAL(r0) = fix->pow(state, as<Bignum>(RVAL(r2)));
      } else if(Fixnum* fix = try_as<Fixnum>(Bignum::normalize(state, as<Bignum>(RVAL(r2))))) {
        RVAL(r0) = as<Bignum>(RVAL(r1))->pow(state, fix);
      } else {
        RVAL(r0) = as<Bignum>(RVAL(r1))->pow(state, as<Bignum>(RVAL(r2)));
      }
    }
  }
}
