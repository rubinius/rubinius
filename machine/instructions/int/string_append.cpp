#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/object.hpp"
#include "builtin/string.hpp"

intptr_t rubinius::int_string_append(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/string_append.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
