#include "builtin/constant_cache.hpp"
#include "builtin/class.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/module.hpp"

#include "object_utils.hpp"
#include "ontology.hpp"

namespace rubinius {
  void ConstantCache::init(STATE) {
    GO(constant_cache_entry).set(
        ontology::new_class(state, "ConstantCacheEntry",
          G(object), G(rubinius)));
    GO(constant_cache).set(
        ontology::new_class(state, "ConstantCache",
          G(object), G(rubinius)));

  }

  ConstantCacheEntry* ConstantCacheEntry::create(STATE, Object* value,
                                                 Module* under,
                                                 ConstantScope* scope) {
    ConstantCacheEntry* cache_entry = state->new_object_dirty<ConstantCacheEntry>(G(constant_cache_entry));

    cache_entry->value(state, value);
    cache_entry->under(state, under);
    cache_entry->scope(state, scope);

    return cache_entry;
  }

  ConstantCache* ConstantCache::create(STATE, Symbol* name, Object* value,
                                             ConstantScope* scope)
  {
    ConstantCache* cache =
      state->vm()->new_object_mature<ConstantCache>(G(constant_cache));

    cache->name(state, name);
    cache->update(state, value, scope);
    return cache;
  }

  ConstantCache* ConstantCache::create(STATE, Symbol* name, Object* value, Module* under,
                                             ConstantScope* scope)
  {
    ConstantCache* cache =
      state->vm()->new_object_mature<ConstantCache>(G(constant_cache));

    cache->name(state, name);
    cache->update(state, value, under, scope);
    return cache;
  }

  ConstantCache* ConstantCache::empty(STATE, Symbol* name) {
    ConstantCache* cache =
      state->vm()->new_object_mature<ConstantCache>(G(constant_cache));

    cache->name(state, name);
    cache->serial_ = -1;
    return cache;
  }

  void ConstantCache::update(STATE, Object* val, ConstantScope* sc) {
    update(state, val, nil<Module>(), sc);
  }

  void ConstantCache::update(STATE, Object* val, Module* mc, ConstantScope* sc) {
    ConstantCacheEntry* cache_entry = ConstantCacheEntry::create(state, val, mc, sc);
    atomic::memory_barrier();
    entry(state, cache_entry);
    serial_ = state->shared().global_serial();
  }
}

