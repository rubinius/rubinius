#include "instructions/send_vcall.hpp"

intptr_t rubinius::int_send_vcall(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t literal = argument(0);

  instruction_send_vcall(state, call_frame, literal);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
