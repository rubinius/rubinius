#include "instructions/push_cpath_top.hpp"

intptr_t rubinius::int_push_cpath_top(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_cpath_top(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
