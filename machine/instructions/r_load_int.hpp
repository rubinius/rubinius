#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_int(CF, R0, R1) {
      REG(r0) = STRIP_FIXNUM_TAG(REG(r1));
    }
  }
}
