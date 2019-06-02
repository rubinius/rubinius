#include "instructions.hpp"

#include "class/block_environment.hpp"
#include "class/proc.hpp"

namespace rubinius {
  namespace instructions {
    inline bool yield_splat(STATE, CallFrame* call_frame, intptr_t count) {
      Object* ary = stack_pop();
      Object* t1 = call_frame->scope->block();
      Object* value;

      Arguments args(G(sym_call), t1, count, stack_back_position(count));

      if(!ary->nil_p()) {
        args.append(state, as<Array>(ary));
      }

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
