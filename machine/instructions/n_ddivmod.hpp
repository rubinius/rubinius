#include "instructions.hpp"

#include "class/float.hpp"

#include <cmath>

namespace rubinius {
  namespace instructions {
    inline void n_ddivmod(STATE, CF, R0, R1, R2) {
      double d = RFLT(r2);

      if(d == 0.0) {
        Exception::raise_zero_division_error(state, "divided by 0");
      }

      double n = RFLT(r1);
      double q = n / d;
      double m = fmod(n, d);

      if((d < 0.0 && n > 0.0 && m != 0) ||
         (d > 0.0 && n < 0.0 && m != 0)) {
        m += d;
      }

      RVAL(r1) = Bignum::from_double(state, floor(q));
      RVAL(r2) = Float::create(state, m);
    }
  }
}
