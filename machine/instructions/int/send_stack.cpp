#include "instructions/send_stack.hpp"

intptr_t rubinius::int_send_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t literal = argument(0);
  intptr_t count = argument(1);

  instruction_send_stack(state, call_frame, literal, count);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
