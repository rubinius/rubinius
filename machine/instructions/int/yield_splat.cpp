#include "instructions/yield_splat.hpp"

intptr_t rubinius::int_yield_splat(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t count = argument(0);

  instruction_yield_splat(state, call_frame, count);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
