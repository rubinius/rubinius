#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_f_binops(CF, R0, R1) {
      RVAL(r0) = call_frame->scope->get_local(0);
      RVAL(r1) = call_frame->scope->get_local(1);
    }
  }
}
