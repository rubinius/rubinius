#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_2(CF, R0) {
      REG(r0) = 2L;
    }
  }
}
