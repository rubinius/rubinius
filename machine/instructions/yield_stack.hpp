#include "instructions.hpp"

#include "class/block_environment.hpp"
#include "class/proc.hpp"

namespace rubinius {
  namespace instructions {
    inline bool yield_stack(STATE, CallFrame* call_frame, intptr_t count) {
      Object* t1 = call_frame->scope->block();
      Arguments args(G(sym_call), t1, count, stack_back_position(count));

      if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
        call_frame->return_value = env->call(state, args);
      } else if(Proc* proc = try_as<Proc>(t1)) {
        call_frame->return_value = proc->yield(state, args);
      } else if(t1->nil_p()) {
        state->raise_exception(Exception::make_lje(state));
        call_frame->return_value = NULL;
      } else {
        Dispatch dispatch(G(sym_call));
        call_frame->return_value = dispatch.send(state, args);
      }

      stack_clear(count);

      state->vm()->checkpoint(state);

      CHECK_AND_PUSH(call_frame->return_value);
    }
  }
}
