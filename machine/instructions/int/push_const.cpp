#include "instructions/push_const.hpp"

intptr_t rubinius::int_push_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t literal = argument(0);

  instruction_push_const(state, call_frame, literal);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
