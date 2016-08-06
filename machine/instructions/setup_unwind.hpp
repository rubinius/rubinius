#include "interpreter/instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void setup_unwind(CallFrame* call_frame, intptr_t ip, intptr_t type) {
      // TODO: needs to be in CallFrame
      UnwindInfoSet unwinds;

      unwinds.push(ip, stack_calculate_sp(), (UnwindType)type);
    }
  }
}
