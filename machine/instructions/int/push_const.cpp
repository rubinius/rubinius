#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"
#include "helpers.hpp"
#include "on_stack.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/autoload.hpp"
#include "builtin/constant_cache.hpp"
#include "builtin/object.hpp"

intptr_t rubinius::int_push_const(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/push_const.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
