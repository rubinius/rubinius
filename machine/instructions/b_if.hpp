#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool b_if(CF, R0) {
      if(REG(r0)) {
        return true;
      } else {
        return false;
      }
    }
  }
}
