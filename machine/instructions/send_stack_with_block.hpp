#include "interpreter/instructions.hpp"

#include "builtin/call_site.hpp"

inline bool rubinius::instruction_send_stack_with_block(STATE, CallFrame* call_frame, intptr_t literal, intptr_t count) {
  Object* block = stack_pop();
  Object* recv = stack_back(count);
  CallSite* call_site = reinterpret_cast<CallSite*>(literal);

  Arguments args(call_site->name(), recv, block, count,
                 stack_back_position(count));

  Object* ret = call_site->execute(state, args);

  stack_clear(count + 1);

  state->vm()->checkpoint(state);

  CHECK_AND_PUSH(ret);
}
