#include "instructions.hpp"

#include <cmath>

namespace rubinius {
  namespace instructions {
    inline void n_dinf(CF, R0, R1) {
      double r = RFLT(r1);

      if(std::isinf(r)) {
        REG(r0) = std::signbit(r) ? -1L : 1L;
      } else {
        REG(r0) = 0L;
      }
    }
  }
}
