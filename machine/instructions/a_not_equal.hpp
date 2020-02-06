#include "instructions.hpp"

#include <sstream>

namespace rubinius {
  namespace instructions {
    inline void a_not_equal(STATE, CF, R0, R1) {
      if(REG(r0) == REG(r1)) {
        std::ostringstream msg;

        msg << "objects are equal: "
            << RVAL(r0)->to_string(state)
            << ", "
            << RVAL(r1)->to_string(state);

        Exception::raise_assertion_error(state, msg.str().c_str());
      }
    }
  }
}
