#include "instructions/string_dup.hpp"

intptr_t rubinius::int_string_dup(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_string_dup(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
