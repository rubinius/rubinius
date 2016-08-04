#include "instructions/make_array.hpp"

intptr_t rubinius::int_make_array(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t count = argument(0);

  instruction_make_array(state, call_frame, count);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
