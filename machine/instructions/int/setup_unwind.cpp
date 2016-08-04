#include "instructions/setup_unwind.hpp"

intptr_t rubinius::int_setup_unwind(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t ip = argument(0);
  intptr_t type = argument(1);

  instruction_setup_unwind(call_frame, ip, type);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
