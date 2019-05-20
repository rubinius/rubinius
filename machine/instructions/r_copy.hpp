#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_copy(CF, R0, R1) {
      REG(r0) = REG(r1);
    }
  }
}
