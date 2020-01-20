#include "instructions.hpp"

#include <cmath>

namespace rubinius {
  namespace instructions {
    inline void n_dnan(CF, R0, R1) {
      REG(r0) = std::isnan(RFLT(r1));
    }
  }
}
