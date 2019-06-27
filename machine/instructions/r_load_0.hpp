#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_0(CF, R0) {
      REG(r0) = 0L;
    }
  }
}
