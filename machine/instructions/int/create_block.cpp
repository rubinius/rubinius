#include "instructions/create_block.hpp"

#include "builtin/block_environment.hpp"
#include "builtin/code_db.hpp"
#include "builtin/object.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"

intptr_t rubinius::int_create_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
  intptr_t literal = argument(0);

  instruction_create_block(state, call_frame, literal);

  return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
}
