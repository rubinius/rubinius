#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_dcmp(CF, R0, R1, R2) {
      double a = RFLT(r1);
      double b = RFLT(r2);

      if(a == b) {
        RVAL(r0) = Fixnum::from(0);
      } else if(a < b) {
        RVAL(r0) = Fixnum::from(-1);
      } else {
        RVAL(r0) = Fixnum::from(1);
      }
    }
  }
}
