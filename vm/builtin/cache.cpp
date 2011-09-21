#include "builtin/cache.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  MethodCacheEntry* MethodCacheEntry::create(STATE, Class* klass, Module* mod,
                                             Executable* exec)
  {
    MethodCacheEntry* cache =
      state->new_object<MethodCacheEntry>(G(object));

    cache->stored_module(state, mod);
    cache->receiver_class(state, klass);
    cache->method(state, exec);

    return cache;
  }
}

