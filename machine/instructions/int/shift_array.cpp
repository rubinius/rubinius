#include "instructions/shift_array.hpp"

intptr_t rubinius::int_shift_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_shift_array(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
