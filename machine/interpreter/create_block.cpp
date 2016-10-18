#include "instructions/create_block.hpp"

#include "class/block_environment.hpp"
#include "class/code_db.hpp"
#include "class/object.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"

namespace rubinius {
  namespace interpreter {
    intptr_t create_block(STATE, CallFrame* call_frame, intptr_t const opcodes[]) {
      intptr_t literal = argument(0);

      if(instructions::create_block(state, call_frame, literal)) {
        call_frame->next_ip(instructions::data_create_block.width);
      } else {
        call_frame->exception_ip();
      }

      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    }
  }
}
