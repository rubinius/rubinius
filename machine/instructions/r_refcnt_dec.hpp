#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_refcnt_dec(STATE, CF, R0) {
      RVAL(r0)->sub_reference(state);
    }
  }
}
