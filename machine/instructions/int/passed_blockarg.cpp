#include "instructions/passed_blockarg.hpp"

intptr_t rubinius::int_passed_blockarg(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t count = argument(0);

  instruction_passed_blockarg(state, call_frame, count);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
