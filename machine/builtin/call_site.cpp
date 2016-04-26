#include "call_frame.hpp"
#include "object_utils.hpp"
#include "memory.hpp"

#include "builtin/class.hpp"
#include "builtin/call_site.hpp"
#include "builtin/exception.hpp"
#include "builtin/executable.hpp"
#include "builtin/object.hpp"

#include <stdlib.h>
#include <sstream>

namespace rubinius {
  int CallSite::max_caches = 0;
  CallSite::Executor CallSite::default_execute = CallSite::lookup_invoke_cache;

  void CallSite::bootstrap(STATE) {
    GO(call_site).set(state->memory()->new_class<Class, CallSite>(
          state, G(rubinius), "CallSite"));

    if(state->shared().config.machine_call_site_cache.value) {
      default_execute = lookup_invoke_cache;
    } else {
      default_execute = dispatch;
    }

    max_caches = state->shared().config.machine_call_site_limit.value;
  }

  void CallSite::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);

    CallSite* call_site = as<CallSite>(obj);

    if(!call_site->caches()) return;

    // 1. Check if individual caches should be evicted.
    bool evict_p[call_site->depth()];

    for(int i = 0; i < call_site->depth(); i++) {
      evict_p[i] = call_site->caches()->cache[i].inefficient_p();
    }

    int evict_count = 0;
    for(int i = 0; i < call_site->depth(); i++) {
      if(evict_p[i]) evict_count++;
    }

    if(evict_count) {
      VM::current()->metrics().machine.inline_cache_evicted += evict_count;

      int new_size = call_site->depth() - evict_count;

      if(new_size == 0) {
        call_site->depth(0);
        delete[] call_site->caches();
        call_site->caches(NULL);

        call_site->execute(CallSite::default_execute);
        call_site->cache_miss(CallSite::default_execute);

        return;
      }

      for(int i = 0, j = 0; i < call_site->depth() && j < new_size; i++) {
        if(!evict_p[i]) {
          call_site->caches()->cache[j++] = call_site->caches()->cache[i];
        }
      }

      call_site->caches()->depth(new_size);
    }

    // 2. Attempt to re-order the caches by bubbling most hit forward.
    bool reorder_p = false;
    int indexes[call_site->depth()];

    for(int i = 0; i < call_site->depth(); i++) {
      indexes[i] = i;
    }

    InlineCaches* caches = call_site->caches();

    for(int i = 0; i < call_site->depth() - 1; i++) {
      if(caches->cache[i].hits() < caches->cache[i + 1].hits()) {
        int tmp = indexes[i];
        indexes[i] = indexes[i + 1];
        indexes[i + 1] = tmp;
        reorder_p = true;

        // TODO: pass State through the GC!
        VM::current()->metrics().machine.inline_cache_reordered++;
      }
    }

    if(reorder_p) {
      InlineCache* inline_caches = static_cast<InlineCache*>(
          alloca(sizeof(CallSite) * call_site->depth()));

      for(int i = 0; i < call_site->depth(); i++) {
        inline_caches[i] = caches->cache[i];
      }

      for(int i = 0; i < call_site->depth(); i++) {
        caches->cache[i] = inline_caches[indexes[i]];
      }
    }

    // 3. Mark remaining caches.
    for(int i = 0; i < call_site->depth(); i++) {
      InlineCache* cache = &caches->cache[i];

      if(Object* ref = mark.call(cache->receiver_class())) {
        cache->receiver_class(as<Class>(ref));
        mark.just_set(call_site, ref);
      }

      if(Object* ref = mark.call(cache->stored_module())) {
        cache->stored_module(as<Module>(ref));
        mark.just_set(call_site, ref);
      }

      if(Object* ref = mark.call(cache->executable())) {
        cache->executable(as<Executable>(ref));
        mark.just_set(call_site, ref);
      }
    }
  }
}
