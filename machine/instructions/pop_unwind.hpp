#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline bool pop_unwind(STATE, CallFrame* call_frame) {
      if(!call_frame->unwinds->has_unwinds()) {
        Exception::internal_error(state, "unbalanced pop_unwind");
        return false;
      }

      call_frame->unwinds->drop();
      return true;
    }
  }
}
