#include "instructions/swap.hpp"

intptr_t rubinius::int_swap(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_swap(call_frame);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
