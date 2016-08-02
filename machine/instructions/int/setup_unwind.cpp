#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/object.hpp"

intptr_t rubinius::int_setup_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  // TODO: needs to be in CallFrame
  UnwindInfoSet unwinds;

#include "instructions/setup_unwind.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
