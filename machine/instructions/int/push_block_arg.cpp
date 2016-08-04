#include "instructions/push_block_arg.hpp"

intptr_t rubinius::int_push_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_block_arg(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
