#include "instructions/pop_unwind.hpp"

intptr_t rubinius::int_pop_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_pop_unwind(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
