#include "instructions/push_type.hpp"

intptr_t rubinius::int_push_type(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_type(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
