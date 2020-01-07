#include "instructions.hpp"

#include "class/float.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_float(CF, R0, R1) {
      RFLT(r0) = as<Float>(RVAL(r1))->value();
    }
  }
}
