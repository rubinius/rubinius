#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_neg1(CF, R0) {
      REG(r0) = -1L;
    }
  }
}
