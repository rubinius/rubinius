#include "call_frame.hpp"
#include "object_utils.hpp"
#include "memory.hpp"

#include "class/class.hpp"
#include "class/call_site.hpp"
#include "class/exception.hpp"
#include "class/executable.hpp"
#include "class/object.hpp"
#include "class/tuple.hpp"

#include <stdlib.h>
#include <sstream>

namespace rubinius {
  int CallSite::max_caches = 0;
  int CallSite::max_evictions = 0;

  void CallSite::bootstrap(STATE) {
    GO(call_site).set(state->memory()->new_class<Class, CallSite>(
          state, G(rubinius), "CallSite"));

    max_caches = state->shared().config.machine_call_site_cache_limit.value;
    max_evictions = state->shared().config.machine_call_site_eviction_limit.value;
  }

  Tuple* CallSite::cache_entries(STATE) {
    Tuple* caches = nullptr;

    if(Cache* cache = this->cache()) {
      caches = Tuple::create(state, cache->size());

      for(int i = 0; i < cache->size(); i++) {
        Cache::Entry* entry = cache->entries(i);

        Tuple* t = Tuple::from(state, 8,
              state->symbol("receiver"),
              entry->receiver_class()->module_name(),
              state->symbol("prediction"),
              entry->prediction(),
              state->symbol("hits"),
              Integer::from(state, entry->hits()),
              state->symbol("misses"),
              Integer::from(state, entry->misses())
            );

        caches->put(state, i, t);
      }
    } else {
      caches = Tuple::create(state, 0);
    }

    return caches;
  }

  void CallSite::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);

    CallSite* call_site = as<CallSite>(obj);

    call_site->evict_and_mark(mark);
  }
}
