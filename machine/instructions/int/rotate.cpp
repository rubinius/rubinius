#include "instructions/rotate.hpp"

intptr_t rubinius::int_rotate(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t count = argument(0);

  instruction_rotate(call_frame, count);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
