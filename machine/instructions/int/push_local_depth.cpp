#include "instructions/push_local_depth.hpp"

intptr_t rubinius::int_push_local_depth(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t depth = argument(0);
  intptr_t index = argument(1);

  instruction_push_local_depth(state, call_frame, depth, index);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
