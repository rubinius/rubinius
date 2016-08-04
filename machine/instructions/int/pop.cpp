#include "instructions/pop.hpp"

intptr_t rubinius::int_pop(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_pop(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
