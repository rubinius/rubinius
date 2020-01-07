#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_dneg(CF, R0, R1) {
      RFLT(r0) = -RFLT(r1);
    }
  }
}
