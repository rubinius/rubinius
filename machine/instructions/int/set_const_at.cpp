#include "instructions/set_const_at.hpp"

intptr_t rubinius::int_set_const_at(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t literal = argument(0);

  instruction_set_const_at(state, call_frame, literal);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
