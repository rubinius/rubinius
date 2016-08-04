#include "instructions/push_false.hpp"

intptr_t rubinius::int_push_false(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_false(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
