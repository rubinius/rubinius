#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_int(STATE, CF, R0, R1) {
      intptr_t r = REG(r1);

      if(!reinterpret_cast<Fixnum*>(r)->fixnum_p()) {
        Exception::raise_type_error(state, "r_load_int argument is not a Fixnum");
      }

      REG(r0) = STRIP_FIXNUM_TAG(r);
    }
  }
}
