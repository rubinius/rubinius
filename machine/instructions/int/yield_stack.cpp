#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/block_environment.hpp"
#include "builtin/object.hpp"
#include "builtin/proc.hpp"

intptr_t rubinius::int_yield_stack(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/yield_stack.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
