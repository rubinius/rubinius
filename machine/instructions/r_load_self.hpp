#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_self(CF, R0) {
      RVAL(r0) = call_frame->self();
    }
  }
}
