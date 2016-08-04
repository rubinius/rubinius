#include "instructions/kind_of.hpp"

intptr_t rubinius::int_kind_of(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_kind_of(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
