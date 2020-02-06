#include "instructions.hpp"

#include <sstream>

namespace rubinius {
  namespace instructions {
    inline void a_less(STATE, CF, R0, R1) {
      Object* v0 = RVAL(r0);
      Object* v1 = RVAL(r1);

      if(!v0->integer_p()) {
        std::ostringstream msg;

        msg << "object is not an Integer: " << v0->to_string(state);

        Exception::raise_assertion_error(state, msg.str().c_str());
      }

      if(!v1->integer_p()) {
        std::ostringstream msg;

        msg << "object is not an Integer: " << v1->to_string(state);

        Exception::raise_assertion_error(state, msg.str().c_str());
      }

      bool lt = false;

      if(v0->fixnum_p()) {
        if(v1->fixnum_p()) {
          lt = v0 < v1;
        } else {
          lt = CBOOL(as<Fixnum>(v0)->lt(state, as<Bignum>(v1)));
        }
      } else if(RVAL(r1)->fixnum_p()) {
        lt = CBOOL(as<Bignum>(v0)->lt(state, as<Fixnum>(v1)));
      } else {
        lt = CBOOL(as<Bignum>(v0)->lt(state, as<Bignum>(v1)));
      }

      if(!lt) {
        std::ostringstream msg;

        msg << "first object is not less than second: "
            << v0->to_string(state)
            << ", "
            << v1->to_string(state);

        Exception::raise_assertion_error(state, msg.str().c_str());
      }
    }
  }
}
