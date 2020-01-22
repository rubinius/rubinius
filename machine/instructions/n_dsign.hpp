#include "instructions.hpp"

#include <cmath>

namespace rubinius {
  namespace instructions {
    inline void n_dsign(CF, R0, R1) {
      REG(r0) = std::signbit(RFLT(r1)) ? -1L : 1L;
    }
  }
}
