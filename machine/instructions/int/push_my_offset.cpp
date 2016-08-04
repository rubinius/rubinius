#include "instructions/push_my_offset.hpp"

intptr_t rubinius::int_push_my_offset(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t index = argument(0);

  instruction_push_my_offset(call_frame, index);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
