#include "instructions/push_current_exception.hpp"

intptr_t rubinius::int_push_current_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_current_exception(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
