#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_refcnt_inc(STATE, CF, R0) {
      RVAL(r0)->add_reference(state);
    }
  }
}
