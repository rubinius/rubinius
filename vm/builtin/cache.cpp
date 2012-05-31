#include "builtin/cache.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  MethodCacheEntry* MethodCacheEntry::create(STATE, Class* klass, Module* mod,
                                             Executable* exec, MethodMissingReason method_missing,
                                             bool super)
  {
    MethodCacheEntry* cache =
      state->new_object<MethodCacheEntry>(G(object));

    cache->stored_module(state, mod);
    cache->receiver_class(state, klass);
    cache->method(state, exec);
    cache->method_missing_ = method_missing;
    cache->super_ = super;

    return cache;
  }
}

