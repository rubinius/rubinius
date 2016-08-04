#include "instructions/string_append.hpp"

intptr_t rubinius::int_string_append(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_string_append(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
