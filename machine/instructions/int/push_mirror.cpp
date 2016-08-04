#include "instructions/push_mirror.hpp"

intptr_t rubinius::int_push_mirror(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_mirror(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
