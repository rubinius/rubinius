#include "instructions/ret.hpp"

intptr_t rubinius::int_ret(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  return instruction_ret(state, call_frame);
}
