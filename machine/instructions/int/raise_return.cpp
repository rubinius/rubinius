#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/location.hpp"
#include "builtin/object.hpp"

intptr_t rubinius::int_raise_return(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/raise_return.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
