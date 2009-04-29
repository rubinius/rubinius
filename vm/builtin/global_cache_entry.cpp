#include "builtin/global_cache_entry.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  void GlobalCacheEntry::init(STATE) {
    GO(global_cache_entry).set(
        state->new_class("GlobalCacheEntry", G(object), G(rubinius)));
  }

  GlobalCacheEntry* GlobalCacheEntry::create(STATE, Object *value) {
    GlobalCacheEntry *entry =
      state->new_object<GlobalCacheEntry>(G(global_cache_entry));

    entry->update(state, value);
    return entry;
  }

  bool GlobalCacheEntry::valid_p(STATE) {
    return serial_->to_native() == state->shared.global_serial();
  }

  void GlobalCacheEntry::update(STATE, Object* val) {
    value(state, val);
    serial(state, Fixnum::from(state->shared.global_serial()));
  }
}

