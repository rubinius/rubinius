#include "instructions.hpp"

#include "object_utils.hpp"

#include "class/array.hpp"

namespace rubinius {
  namespace instructions {
    inline bool cast_array(STATE, CallFrame* call_frame) {
      // Use stack_top and not stack_pop because we may need
      // to preserve and reread the value from the stack below.
      Object* t1 = stack_top();
      if(t1->nil_p()) {
        t1 = Array::create(state, 0);
      } else if(Tuple* tup = try_as<Tuple>(t1)) {
        t1 = Array::from_tuple(state, tup);
      } else if(!rubinius::kind_of<Array>(t1)) {
        Object* recv = G(type);
        Arguments args(G(sym_coerce_to_array), recv, 1, &t1);
        Dispatch dispatch(G(sym_coerce_to_array));

        Object* res = dispatch.send(state, args);

        // If the send still doesn't produce an array, wrap
        // the value in one.
        if(res && !rubinius::kind_of<Array>(res)) {
          Array* ary = Array::create(state, 1);
          // Don't read t1 here, it's not GC safe because we called
          // a method.
          ary->set(state, 0, stack_top());
          t1 = ary;
        } else {
          t1 = res;
        }
      }

      (void)stack_pop(); // Remove original value

      CHECK_AND_PUSH(t1);
    }
  }
}
