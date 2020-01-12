#include "instructions.hpp"

#include "class/bignum.hpp"
#include "class/float.hpp"

namespace rubinius {
  namespace instructions {
    inline void n_demote(STATE, CF, R0, R1, R2) {
      if(try_as<Float>(RVAL(r1))) {
        // We don't overwrite any register if the 2nd value isn't valid
        if(try_as<Float>(RVAL(r2))) {
          RFLT(r1) = as<Float>(RVAL(r1))->value();
          RFLT(r2) = as<Float>(RVAL(r2))->value();
          REG(r0) = 1;
        } else if(try_as<Bignum>(RVAL(r2))) {
          Integer* i = Bignum::from_double(state, as<Float>(RVAL(r1))->value());
          if(Fixnum* a = try_as<Fixnum>(i)) {
            if(Fixnum* b = try_as<Fixnum>(Bignum::normalize(state, as<Bignum>(RVAL(r2))))) {
              RVAL(r1) = a;
              RVAL(r2) = b;
              REG(r0) = 3;
            } else {
              RVAL(r1) = i;
              REG(r0) = 2;
            }
          } else {
            RVAL(r1) = i;
            REG(r0) = 2;
          }
        } else if(RVAL(r2)->fixnum_p()) {
          Integer* i = Bignum::from_double(state, as<Float>(RVAL(r1))->value());
          if(Fixnum* a = try_as<Fixnum>(i)) {
            RVAL(r1) = a;
            REG(r0) = 3;
          } else {
            RVAL(r1) = a;
            RVAL(r2) = Bignum::from(state, STRIP_FIXNUM_TAG(RVAL(r2)));
            REG(r0) = 2;
          }
        } else {
          REG(r0) = 0;
        }
      } else if(try_as<Bignum>(RVAL(r1))) {
        if(try_as<Float>(RVAL(r2))) {
          Integer* i = Bignum::from_double(state, as<Float>(RVAL(r2))->value());
          if(Fixnum* a = try_as<Fixnum>(i)) {
            if(Fixnum* b = try_as<Fixnum>(Bignum::normalize(state, as<Bignum>(RVAL(r1))))) {
              RVAL(r1) = b;
              RVAL(r2) = a;
              REG(r0) = 3;
            } else {
              RVAL(r2) = i;
              REG(r0) = 2;
            }
          } else {
            RVAL(r2) = i;
            REG(r0) = 2;
          }
        } else if(try_as<Bignum>(RVAL(r2))) {
          REG(r0) = 2;
        } else if(RVAL(r2)->fixnum_p()) {
          if(Fixnum* b = try_as<Fixnum>(Bignum::normalize(state, as<Bignum>(RVAL(r1))))) {
            RVAL(r1) = b;
            REG(r0) = 3;
          } else {
            RVAL(r2) = Bignum::from(state, STRIP_FIXNUM_TAG(RVAL(r2)));
            REG(r0) = 2;
          }
        } else {
          REG(r0) = 0;
        }
      } else if(RVAL(r1)->fixnum_p()) {
        if(try_as<Float>(RVAL(r2))) {
          Integer* i = Bignum::from_double(state, as<Float>(RVAL(r2))->value());
          if(Fixnum* a = try_as<Fixnum>(i)) {
            RVAL(r2) = a;
            REG(r0) = 3;
          } else {
            RVAL(r1) = Bignum::from(state, STRIP_FIXNUM_TAG(RVAL(r1)));
            RVAL(r2) = a;
            REG(r0) = 2;
          }
        } else if(try_as<Bignum>(RVAL(r2))) {
          if(Fixnum* b = try_as<Fixnum>(Bignum::normalize(state, as<Bignum>(RVAL(r2))))) {
            RVAL(r2) = b;
            REG(r0) = 3;
          } else {
            RVAL(r1) = Bignum::from(state, STRIP_FIXNUM_TAG(RVAL(r1)));
            REG(r0) = 2;
          }
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
