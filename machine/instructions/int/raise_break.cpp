#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/location.hpp"
#include "builtin/object.hpp"

intptr_t rubinius::int_raise_break(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/raise_break.hpp"
}
