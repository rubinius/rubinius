#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool b_if_int(CallFrame* call_frame, const intptr_t r0, const intptr_t r1) {
      if(fixnums_p(REG(r0), REG(r1))) {
        return true;
      } else {
        return false;
      }
    }
  }
}
