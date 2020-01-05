#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_dge(CF, R0, R1, R2) {
      RFLT(r0) = RFLT(r1) >= RFLT(r2);
    }
  }
}
