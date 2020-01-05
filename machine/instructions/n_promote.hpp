#include "instructions.hpp"

#include "class/bignum.hpp"
#include "class/float.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_promote(STATE, CF, R0, R1, R2) {
      if(try_as<Float>(RVAL(r1))) {
        // We don't overwrite any register if the 2nd value isn't valid
        if(try_as<Float>(RVAL(r2))) {
          RFLT(r1) = as<Float>(RVAL(r1))->value();
          RFLT(r2) = as<Float>(RVAL(r2))->value();
          REG(r0) = 1;
        } else if(try_as<Bignum>(RVAL(r2))) {
          RFLT(r1) = as<Float>(RVAL(r1))->value();
          RFLT(r2) = as<Bignum>(RVAL(r2))->to_double(state);
          REG(r0) = 1;
        } else if(RVAL(r2)->fixnum_p()) {
          RFLT(r1) = as<Float>(RVAL(r1))->value();
          RFLT(r2) = (double)(STRIP_FIXNUM_TAG(RVAL(r2)));
          REG(r0) = 1;
        } else {
          REG(r0) = 0;
        }
      } else if(try_as<Bignum>(RVAL(r1))) {
        if(try_as<Float>(RVAL(r2))) {
          RFLT(r1) = as<Bignum>(RVAL(r1))->to_double(state);
          RFLT(r2) = as<Float>(RVAL(r2))->value();
          REG(r0) = 1;
        } else if(try_as<Bignum>(RVAL(r2))) {
          REG(r0) = 2;
        } else if(RVAL(r2)->fixnum_p()) {
          RVAL(r2) = Bignum::from(state, STRIP_FIXNUM_TAG(RVAL(r2)));
          REG(r0) = 2;
        } else {
          REG(r0) = 0;
        }
      } else if(RVAL(r1)->fixnum_p()) {
        if(try_as<Float>(RVAL(r2))) {
          RFLT(r1) = (double)(STRIP_FIXNUM_TAG(RVAL(r1)));
          RFLT(r2) = as<Float>(RVAL(r2))->value();
          REG(r0) = 1;
        } else if(try_as<Bignum>(RVAL(r2))) {
          RVAL(r1) = Bignum::from(state, STRIP_FIXNUM_TAG(RVAL(r1)));
          REG(r0) = 2;
        } else if(RVAL(r2)->fixnum_p()) {
          REG(r0) = 3;
        } else {
          REG(r0) = 0;
        }
      } else {
        REG(r0) = 0;
      }
    }
  }
}
