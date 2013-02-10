#include "builtin/global_cache_entry.hpp"
#include "builtin/class.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/module.hpp"

#include "object_utils.hpp"
#include "ontology.hpp"

namespace rubinius {
  void GlobalCacheEntry::init(STATE) {
    GO(global_cache_entry).set(
        ontology::new_class(state, "GlobalCacheEntry",
          G(object), G(rubinius)));
  }

  GlobalCacheEntry* GlobalCacheEntry::create(STATE, Object* value,
                                             ConstantScope* scope)
  {
    GlobalCacheEntry *entry =
      state->vm()->new_object_mature<GlobalCacheEntry>(G(global_cache_entry));

    entry->update(state, value, scope);
    return entry;
  }

  GlobalCacheEntry* GlobalCacheEntry::create(STATE, Object* value, Module* under,
                                             ConstantScope* scope)
  {
    GlobalCacheEntry *entry =
      state->vm()->new_object_mature<GlobalCacheEntry>(G(global_cache_entry));

    entry->update(state, value, under, scope);
    return entry;
  }

  GlobalCacheEntry* GlobalCacheEntry::empty(STATE) {
    GlobalCacheEntry *entry =
      state->vm()->new_object_mature<GlobalCacheEntry>(G(global_cache_entry));

    entry->serial_ = -1;
    return entry;
  }

  bool GlobalCacheEntry::valid_p(STATE, ConstantScope* scope) {
    return serial_ == state->shared().global_serial() &&
           scope_ == scope;
  }

  bool GlobalCacheEntry::valid_p(STATE, Module* under, ConstantScope* scope) {
    return serial_ == state->shared().global_serial() &&
           under_ == under &&
           scope_ == scope;
  }

  void GlobalCacheEntry::update(STATE, Object* val, ConstantScope* sc) {
    value(state, val);
    scope(state, sc);
    under(state, nil<Module>());
    serial_ = state->shared().global_serial();
  }

  void GlobalCacheEntry::update(STATE, Object* val, Module* mc, ConstantScope* sc) {
    value(state, val);
    scope(state, sc);
    under(state, mc);
    serial_ = state->shared().global_serial();
  }
}

