#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void r_load_literal(CF, R0, LITERAL) {
      REG(r0) = literal;
    }
  }
}
