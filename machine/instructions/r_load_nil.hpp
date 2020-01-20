#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_nil(CF, R0, OBJECT) {
      RVAL(r0) = reinterpret_cast<Object*>(object);
    }
  }
}
