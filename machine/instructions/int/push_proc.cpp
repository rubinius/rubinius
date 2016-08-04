#include "instructions/push_proc.hpp"

intptr_t rubinius::int_push_proc(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_push_proc(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
