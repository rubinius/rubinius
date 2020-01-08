#include "instructions.hpp"

#include "class/float.hpp"

namespace rubinius {
  namespace instructions {
    inline bool b_if_float(CF, R0, R1) {
      if(false) {
        return true;
      } else {
        return false;
      }
    }
  }
}
