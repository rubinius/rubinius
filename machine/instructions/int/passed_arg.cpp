#include "instructions/passed_arg.hpp"

intptr_t rubinius::int_passed_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t index = argument(0);

  instruction_passed_arg(state, call_frame, index);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
