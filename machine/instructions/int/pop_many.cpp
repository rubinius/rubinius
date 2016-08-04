#include "instructions/pop_many.hpp"

intptr_t rubinius::int_pop_many(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t count = argument(0);

  instruction_pop_many(call_frame, count);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
