#include "instructions/clear_exception.hpp"

intptr_t rubinius::int_clear_exception(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_clear_exception(state);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
