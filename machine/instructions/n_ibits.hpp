#include "instructions.hpp"

#include <cmath>

namespace rubinius {
  namespace instructions {
    inline void n_ibits(CF, R0, R1) {
      intptr_t r = REG(r1);

      REG(r0) = static_cast<intptr_t>(ceil(log2(r < 0 ? -r : r+1)));
    }
  }
}
