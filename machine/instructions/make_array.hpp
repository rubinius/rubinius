#include "instructions.hpp"

namespace rubinius {
  namespace instructions {
    inline void make_array(STATE, CallFrame* call_frame, intptr_t count) {
      Object* t2;
      Array* ary = Array::create(state, count);
      Tuple* tup = ary->tuple();

#ifdef RBX_ALLOC_TRACKING
      if(unlikely(state->allocation_tracking())) {
        ary->setup_allocation_site(state);
      }
#endif
      int j = count - 1;
      for(; j >= 0; j--) {
        t2 = stack_pop();
        tup->put(state, j, t2);
      }
      ary->total(state, Fixnum::from(count));
      stack_push(ary);
    }
  }
}
