#include "instructions/raise_return.hpp"

intptr_t rubinius::int_raise_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_raise_return(state, call_frame);
  call_frame->exception_ip();

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
