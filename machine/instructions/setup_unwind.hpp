#include "instructions.hpp"

#include "class/unwind_site.hpp"

namespace rubinius {
  namespace instructions {
    inline void setup_unwind(CallFrame* call_frame, intptr_t ip, intptr_t type) {
      UnwindSite* unwind_site = reinterpret_cast<UnwindSite*>(ip);

      unwind_site->stack_depth(stack_calculate_sp());
      call_frame->push_unwind(unwind_site);
    }
  }
}
