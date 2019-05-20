#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_local(CF, R0, LOCAL) {
      RVAL(r0) = call_frame->scope->get_local(local);
    }
  }
}
