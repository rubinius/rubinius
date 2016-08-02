#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"

#include "interpreter/instructions.hpp"

#include "builtin/call_site.hpp"
#include "builtin/object.hpp"

intptr_t rubinius::int_send_super_stack_with_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
#include "instructions/send_super_stack_with_block.hpp"

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
