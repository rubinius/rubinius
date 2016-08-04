#include "instructions/push_exception_state.hpp"

intptr_t rubinius::int_push_exception_state(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_exception_state(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
