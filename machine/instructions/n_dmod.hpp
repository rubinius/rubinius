#include "instructions.hpp"

#include <cmath>

namespace rubinius {
  namespace instructions {
    inline void n_dmod(STATE, CF, R0, R1, R2) {
      double d = RFLT(r2);

      if(d == 0.0) {
        Exception::raise_zero_division_error(state, "divided by 0");
      }

      RFLT(r0) = fmod(RFLT(r1), d);
    }
  }
}
