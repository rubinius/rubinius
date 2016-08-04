#include "instructions/noop.hpp"

intptr_t rubinius::int_noop(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_noop();

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
