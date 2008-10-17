#include "builtin/contexts.hpp"
#include "objectmemory.hpp"
#include "context_cache.hpp"

namespace rubinius {

  ContextCache::ContextCache() {
    reset();
  }

  /* Reset all state to empty. */
  void ContextCache::reset() {
    reclaim = 0;
    for(size_t i = 0; i < CountContextCaches; i++) {
      caches[i] = (MethodContext*)Qnil;
    }
  }

};

