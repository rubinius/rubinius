#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_store_stack(CF, R0) {
      stack_push(RVAL(r0));
    }
  }
}
