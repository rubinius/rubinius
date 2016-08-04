#include "instructions/ensure_return.hpp"

intptr_t rubinius::int_ensure_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_ensure_return(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
