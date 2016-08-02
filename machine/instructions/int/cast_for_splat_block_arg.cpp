#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/object.hpp"

intptr_t rubinius::int_cast_for_splat_block_arg(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/cast_for_splat_block_arg.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
