#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/call_site.hpp"
#include "builtin/object.hpp"
#include "builtin/variable_scope.hpp"

intptr_t rubinius::int_zsuper(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/zsuper.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
