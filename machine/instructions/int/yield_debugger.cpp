#include "instructions/yield_debugger.hpp"

intptr_t rubinius::int_yield_debugger(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  instruction_yield_debugger(state);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
