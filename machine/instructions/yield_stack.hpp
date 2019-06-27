#include "instructions.hpp"

#include "class/block_environment.hpp"
#include "class/proc.hpp"

namespace rubinius {
  namespace instructions {
    inline bool yield_stack(STATE, CallFrame* call_frame, intptr_t count) {
      Object* t1 = call_frame->scope->block();
      Arguments args(G(sym_call), t1, count, stack_back_position(count));
      Object* value;

      if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
        value = env->call(state, args);
      } else if(Proc* proc = try_as<Proc>(t1)) {
        value = proc->yield(state, args);
      } else if(t1->nil_p()) {
        state->raise_exception(Exception::make_lje(state));
        value = nullptr;
      } else {
        Dispatch dispatch(G(sym_call));
        value = dispatch.send(state, args);
      }

      stack_clear(count);

      CHECK_AND_PUSH(value);
    }
  }
}
