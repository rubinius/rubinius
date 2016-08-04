#include "instructions/push_scope.hpp"

intptr_t rubinius::int_push_scope(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_scope(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
