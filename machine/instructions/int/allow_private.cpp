#include "instructions/allow_private.hpp"

intptr_t rubinius::int_allow_private(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_allow_private();

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
