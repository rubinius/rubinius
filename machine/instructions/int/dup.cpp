#include "instructions/dup.hpp"

intptr_t rubinius::int_dup(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_dup(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
