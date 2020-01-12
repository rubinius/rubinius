#include "instructions.hpp"

#include "class/float.hpp"

namespace rubinius {
  namespace instructions {
    inline bool b_if_float(CF, R0, R1) {
      if(try_as<Float>(RVAL(r0)) && try_as<Float>(RVAL(r1))) {
        return true;
      } else {
        return false;
      }
    }
  }
}
