#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/object.hpp"
#include "builtin/thread_state.hpp"

intptr_t rubinius::int_push_exception_state(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/push_exception_state.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
