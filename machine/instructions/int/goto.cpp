#include "instructions/goto.hpp"

intptr_t rubinius::int_goto(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t location = argument(0);

  instruction_goto(call_frame, location);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
