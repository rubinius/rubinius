#include "instructions/push_undef.hpp"

intptr_t rubinius::int_push_undef(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_undef(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
