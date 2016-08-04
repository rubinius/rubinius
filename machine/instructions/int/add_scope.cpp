#include "instructions/add_scope.hpp"

intptr_t rubinius::int_add_scope(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_add_scope(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
