#include "instructions/cast_multi_value.hpp"

intptr_t rubinius::int_cast_multi_value(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  if(instruction_cast_multi_value(state, call_frame)) {
    call_frame->next_ip();
  } else {
    call_frame->exception_ip();
  }

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
