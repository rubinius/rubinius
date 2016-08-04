#include "instructions/push_true.hpp"

intptr_t rubinius::int_push_true(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_true(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
