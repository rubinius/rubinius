#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline bool b_if_eint(CF, R0, R1) {
      if(false) {
        return true;
      } else {
        return false;
      }
    }
  }
}
