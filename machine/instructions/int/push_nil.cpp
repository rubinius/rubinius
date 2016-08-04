#include "instructions/push_nil.hpp"

intptr_t rubinius::int_push_nil(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_nil(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
