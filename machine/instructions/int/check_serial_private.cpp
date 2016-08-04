#include "instructions/check_serial_private.hpp"

intptr_t rubinius::int_check_serial_private(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t literal = argument(0);
  intptr_t serial = argument(1);

  instruction_check_serial_private(state, call_frame, literal, serial);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
