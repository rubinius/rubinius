#include "instructions/push_local.hpp"

intptr_t rubinius::int_push_local(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t local = argument(0);

  instruction_push_local(call_frame, local);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
