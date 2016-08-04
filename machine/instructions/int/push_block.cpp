#include "instructions/push_block.hpp"

intptr_t rubinius::int_push_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_block(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
