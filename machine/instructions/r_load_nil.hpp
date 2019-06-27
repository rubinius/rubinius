#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_nil(CF, R0, RVALUE) {
      RVAL(r0) = reinterpret_cast<Object*>(rvalue);
    }
  }
}
