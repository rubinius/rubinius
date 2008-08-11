#include "builtin/contexts.hpp"
#include "objectmemory.hpp"
#include "context_cache.hpp"

namespace rubinius {
  /* Reset all state to empty. */
  void ContextCache::reset() {
    reclaim = 0;
    for(size_t i = 0; i < CountContextCaches; i++) {
      caches[i] = (MethodContext*)Qnil;
    }
  }

  /* Retrieve a context from +which+ cache. Return NULL if the cache
   * was empty. */
  MethodContext* ContextCache::get(size_t which) {
    MethodContext* ctx;
    if((ctx = caches[which]) != Qnil) {
      /* We chain off ->home */
      caches[which] = ctx->home;
      return ctx;
    }

    return NULL;
  }

  /* Add context +ctx+ to cache +which+. */
  void ContextCache::add(size_t which, MethodContext* ctx) {
    ctx->home = caches[which];
    caches[which] = ctx;

    reclaim--;
  }

};
