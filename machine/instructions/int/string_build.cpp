#include "instructions/string_build.hpp"

intptr_t rubinius::int_string_build(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t count = argument(0);

  if(instruction_string_build(state, call_frame, count)) {
    call_frame->next_ip();
  } else {
    call_frame->exception_ip();
  }

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
