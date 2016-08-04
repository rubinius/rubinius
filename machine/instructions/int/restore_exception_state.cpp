#include "instructions/restore_exception_state.hpp"

intptr_t rubinius::int_restore_exception_state(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_restore_exception_state(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
