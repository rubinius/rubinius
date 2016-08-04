#include "instructions/push_rubinius.hpp"

intptr_t rubinius::int_push_rubinius(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_rubinius(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
