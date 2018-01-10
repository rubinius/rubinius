#include "instructions.hpp"
#include "class/exception.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_idiv(STATE, CF, R0, R1, R2) {
      if(REG(r2) == 0) {
        Exception::raise_zero_division_error(state, "divided by 0");
      }

      REG(r0) = REG(r1) / REG(r2);
    }
  }
}
