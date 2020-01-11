#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_iflt(CF, R0, R1) {
      RFLT(r0) = (double)STRIP_FIXNUM_TAG(REG(r1));
    }
  }
}
