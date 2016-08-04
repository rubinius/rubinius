#include "interpreter/instructions.hpp"

inline void rubinius::instruction_setup_unwind(CallFrame* call_frame, intptr_t ip, intptr_t type) {
  // TODO: needs to be in CallFrame
  UnwindInfoSet unwinds;

  unwinds.push(ip, stack_calculate_sp(), (UnwindType)type);
}
