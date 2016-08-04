#include "instructions/send_method.hpp"

intptr_t rubinius::int_send_method(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t literal = argument(0);

  instruction_send_method(state, call_frame, literal);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
