#include "instructions.hpp"

#include "class/bignum.hpp"

namespace rubinius {
  namespace instructions {
    inline bool b_if_eint(CF, R0, R1) {
      if(try_as<Bignum>(RVAL(r0)) && try_as<Bignum>(RVAL(r1))) {
        return true;
      } else {
        return false;
      }
    }
  }
}
