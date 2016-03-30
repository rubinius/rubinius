#include "object_utils.hpp"
#include "memory.hpp"

#include "builtin/constant_cache.hpp"
#include "builtin/class.hpp"
#include "builtin/constant_scope.hpp"

namespace rubinius {
  void ConstantCache::bootstrap(STATE) {
    GO(constant_cache).set(state->memory()->new_class<Class, ConstantCache>(
          state, G(rubinius), "ConstantCache"));
  }

  ConstantCache* ConstantCache::create(STATE, ConstantCache* existing,
      Object* value, Module* mod, ConstantScope* scope)
  {
    ConstantCache* cache =
      state->memory()->new_object<ConstantCache>(state, G(constant_cache));

    cache->name(state, existing->name());
    cache->executable(state, existing->executable());
    cache->ip(existing->ip());
    cache->value(state, value);
    cache->module(state, mod);
    cache->constant_scope(state, scope);
    cache->serial(state->shared().global_serial());

    return cache;
  }

  ConstantCache* ConstantCache::create(STATE, ConstantCache* existing, Object* value,
                                             ConstantScope* scope) {
    return create(state, existing, value, nil<Module>(), scope);
  }

  ConstantCache* ConstantCache::empty(STATE, Symbol* name, Executable* executable, int ip) {
    ConstantCache* cache =
      state->memory()->new_object<ConstantCache>(state, G(constant_cache));

    cache->name(state, name);
    cache->executable(state, executable);
    cache->ip(ip);
    cache->value(state, cNil);
    cache->module(state, nil<Module>());
    cache->constant_scope(state, nil<ConstantScope>());
    cache->serial(-1);

    return cache;
  }

  Integer* ConstantCache::ip_prim(STATE) {
    return Integer::from(state, ip());
  }

  Integer* ConstantCache::serial_prim(STATE) {
    return Integer::from(state, serial());
  }
}

