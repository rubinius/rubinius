#include "instructions.hpp"

#include "class/call_site.hpp"

namespace rubinius {
  namespace instructions {
    inline bool send_super_stack_with_block(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count) {
      Object* block = stack_pop();
      CallSite* call_site = reinterpret_cast<CallSite*>(literal);
      Object* const recv = call_frame->self();

      Arguments new_args(call_site->name(), recv, block, count,
                         stack_back_position(count));

      stack_clear(count);

      Object* value = call_site->execute(state, new_args);

      CHECK_AND_PUSH(value);
    }
  }
}
