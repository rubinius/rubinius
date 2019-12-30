#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_bool(CF, R0, R1) {
      RVAL(r0) = RBOOL(REG(r1));
    }
  }
}
