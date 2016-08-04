#include "instructions/instance_of.hpp"

intptr_t rubinius::int_instance_of(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_instance_of(state, call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
