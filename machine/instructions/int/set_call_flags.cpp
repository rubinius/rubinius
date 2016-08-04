#include "instructions/set_call_flags.hpp"

intptr_t rubinius::int_set_call_flags(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_set_call_flags(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
