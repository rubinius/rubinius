#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline intptr_t p_return(STATE, CallFrame* call_frame) {
      return reinterpret_cast<intptr_t>(Fixnum::from(0));
    }
  }
}
