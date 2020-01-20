#include "instructions.hpp"

#include <cmath>

namespace rubinius {
  namespace instructions {
    inline void n_dclass(CF, R0, R1) {
      double r = RFLT(r1);

      switch(std::fpclassify(r)) {
        case FP_INFINITE:
          REG(r0) = std::signbit(r) ? -1L : 1L;
          break;
        case FP_NAN:
          REG(r0) = 2L;
          break;
        case FP_NORMAL:
          REG(r0) = 3L;
          break;
        case FP_SUBNORMAL:
          REG(r0) = 4L;
          break;
        case FP_ZERO:
          REG(r0) = std::signbit(r) ? -5L : 5L;
          break;
        default:
          REG(r0) = 0L;
          break;
      }
    }
  }
}
