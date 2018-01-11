#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool b_if_int(CF, R0, R1) {
      if(fixnums_p(REG(r0), REG(r1))) {
        return true;
      } else {
        return false;
      }
    }
  }
}
