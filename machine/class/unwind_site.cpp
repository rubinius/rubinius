#include "object_utils.hpp"
#include "memory.hpp"

#include "class/class.hpp"
#include "class/unwind_site.hpp"
#include "class/object.hpp"

#include "diagnostics/machine.hpp"

namespace rubinius {
  int UnwindSite::max_caches = 0;

  void UnwindSite::bootstrap(STATE) {
    GO(unwind_site).set(state->memory()->new_class<Class, UnwindSite>(
          state, G(rubinius), "UnwindSite"));

    max_caches = state->configuration()->machine_unwind_site_limit.value;
  }

  UnwindSite* UnwindSite::create(STATE, int ip, UnwindType unwind_type) {
    UnwindSite* cache =
      state->memory()->new_variable_object<UnwindSite>(state, G(unwind_site));

    cache->ip(ip);
    cache->unwind_type(unwind_type);

    state->metrics()->unwind_site_count++;

    return cache;
  }
}
