#include "instructions/send_stack_with_block.hpp"

intptr_t rubinius::int_send_stack_with_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t literal = argument(0);
  intptr_t count = argument(1);

  instruction_send_stack_with_block(state, call_frame, literal, count);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
