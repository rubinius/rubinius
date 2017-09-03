#include "object_utils.hpp"
#include "memory.hpp"

#include "class/class.hpp"
#include "class/unwind_site.hpp"
#include "class/object.hpp"

namespace rubinius {
  int UnwindSite::max_caches = 0;

  void UnwindSite::bootstrap(STATE) {
    GO(unwind_site).set(state->memory()->new_class<Class, UnwindSite>(
          state, G(rubinius), "UnwindSite"));

    max_caches = state->shared().config.machine_unwind_site_limit.value;
  }
}
