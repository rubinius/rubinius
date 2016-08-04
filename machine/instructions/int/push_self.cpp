#include "instructions/push_self.hpp"

intptr_t rubinius::int_push_self(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_self(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
