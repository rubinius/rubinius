#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_store_local(CF, R0, LOCAL) {
      call_frame->scope->set_local(local, RVAL(r0));
    }
  }
}
