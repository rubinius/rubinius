#include "instructions/push_has_block.hpp"

intptr_t rubinius::int_push_has_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_has_block(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
