#include "call_frame.hpp"
#include "object_utils.hpp"
#include "memory.hpp"
#include "machine_code.hpp"

#include "class/class.hpp"
#include "class/call_site.hpp"
#include "class/compiled_code.hpp"
#include "class/exception.hpp"
#include "class/executable.hpp"
#include "class/object.hpp"
#include "class/tuple.hpp"

#include "diagnostics/profiler.hpp"

#include <stdlib.h>
#include <sstream>

namespace rubinius {
  int CallSite::max_caches = 0;
  int CallSite::max_evictions = 0;

  void CallSite::bootstrap(STATE) {
    GO(call_site).set(state->memory()->new_class<Class, CallSite>(
          state, G(rubinius), "CallSite"));

    max_caches = state->configuration()->machine_call_site_cache_limit.value;
    max_evictions = state->configuration()->machine_call_site_eviction_limit.value;
  }

  Tuple* CallSite::cache_entries(STATE) {
    Tuple* caches = nullptr;

    if(Cache* cache = this->cache()) {
      caches = Tuple::create(state, cache->size());

      for(int i = 0; i < cache->size(); i++) {
        Cache::Entry* entry = cache->entries(i);

        Tuple* t = Tuple::from(state, 12,
              state->symbol("receiver"),
              entry->receiver_class()->module_name(),
              state->symbol("prediction"),
              entry->prediction(),
              state->symbol("module"),
              entry->module(),
              state->symbol("executable"),
              entry->executable(),
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

  void CallSite::Info::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    auto_mark(state, obj, f);

    CallSite* call_site = as<CallSite>(obj);

    if(Cache* cache = call_site->cache()) {
      // Disable if call site is unstable for caching
      if(cache->evictions() > max_evictions) {
        call_site->set_cache(nullptr);
        call_site->executor(CallSite::dispatch);

        call_site->delete_cache(cache);

        state->metrics()->inline_cache_disabled++;
      } else {
        // Campact and possibly reset cache
        Cache* new_cache = cache->compact();

        if(new_cache != cache) {
          call_site->set_cache(new_cache);
          call_site->delete_cache(cache);

          if(new_cache) {
            state->metrics()->inline_cache_count++;
          } else {
            call_site->executor(CallSite::dispatch_once);
          }
        }
      }
    }

    // Do not merge conditionals, we may have set _cache_ to nullptr above.
    if(Cache* cache = call_site->cache()) {
      // Re-order more used cache entries to the front
      cache->reorder();

      // Mark caches.
      for(int32_t i = 0; i < cache->size(); i++) {
        Cache::Entry* entry = cache->entries(i);

        f(state, entry->p_receiver_class());
        f(state, entry->p_prediction());
        f(state, entry->p_module());
        f(state, entry->p_executable());

        /* TODO: SymbolTable and Module name
        if(state->shared.profiler()->collecting_p()) {
          if(CompiledCode* code = try_as<CompiledCode>(entry->executable())) {
            if(code->machine_code()->sample_count > state->shared.profiler()->sample_min()) {
              state->shared.profiler()->add_entry(call_site->serial(), call_site->ip(),
                  code->machine_code()->serial(), entry->hits(),
                  entry->receiver_class()->name(), entry->module()->name());
            }
          }
        }
        */
      }
    }

    // Clear dead list
    call_site->clear_dead_list();
  }

  void CallSite::Info::before_visit(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    CallSite* call_site = as<CallSite>(obj);

    if(Cache* cache = call_site->cache()) {
      for(int32_t i = 0; i < cache->size(); i++) {
        Cache::Entry* entry = cache->entries(i);

        f(state, entry->p_receiver_class());
        f(state, entry->p_prediction());
        f(state, entry->p_module());
        f(state, entry->p_executable());
      }
    }
  }
}
