#include "instructions/push_variables.hpp"

intptr_t rubinius::int_push_variables(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_variables(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
