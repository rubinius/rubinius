#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/call_site.hpp"
#include "builtin/object.hpp"

intptr_t rubinius::int_send_vcall(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/send_vcall.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
