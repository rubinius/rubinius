#include "instructions.hpp"

#include "class/float.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_store_float(STATE, CF, R0, R1) {
      RVAL(r0) = Float::create(state, RFLT(r1));
    }
  }
}
