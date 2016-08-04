#include "instructions/check_interrupts.hpp"

intptr_t rubinius::int_check_interrupts(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_check_interrupts(state);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
