#include "instructions/store_my_field.hpp"

intptr_t rubinius::int_store_my_field(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t index = argument(0);

  instruction_store_my_field(state, call_frame, index);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
