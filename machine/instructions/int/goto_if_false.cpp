#include "instructions/goto_if_false.hpp"

intptr_t rubinius::int_goto_if_false(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t location = argument(0);

  instruction_goto_if_false(call_frame, location);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
