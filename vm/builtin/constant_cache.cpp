#include "builtin/constant_cache.hpp"
#include "builtin/class.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/module.hpp"

#include "object_utils.hpp"
#include "ontology.hpp"

namespace rubinius {
  void ConstantCache::init(STATE) {
    GO(constant_cache).set(
        ontology::new_class(state, "ConstantCache",
          G(object), G(rubinius)));
  }

  ConstantCache* ConstantCache::create(STATE, Object* value,
                                             ConstantScope* scope)
  {
    ConstantCache *entry =
      state->vm()->new_object_mature<ConstantCache>(G(constant_cache));

    entry->update(state, value, scope);
    return entry;
  }

  ConstantCache* ConstantCache::create(STATE, Object* value, Module* under,
                                             ConstantScope* scope)
  {
    ConstantCache *entry =
      state->vm()->new_object_mature<ConstantCache>(G(constant_cache));

    entry->update(state, value, under, scope);
    return entry;
  }

  ConstantCache* ConstantCache::empty(STATE) {
    ConstantCache *entry =
      state->vm()->new_object_mature<ConstantCache>(G(constant_cache));

    entry->serial_ = -1;
    return entry;
  }

  bool ConstantCache::valid_p(STATE, ConstantScope* scope) {
    return serial_ == state->shared().global_serial() &&
           scope_ == scope;
  }

  bool ConstantCache::valid_p(STATE, Module* under, ConstantScope* scope) {
    return serial_ == state->shared().global_serial() &&
           under_ == under &&
           scope_ == scope;
  }

  void ConstantCache::update(STATE, Object* val, ConstantScope* sc) {
    value(state, val);
    scope(state, sc);
    under(state, nil<Module>());
    serial_ = state->shared().global_serial();
  }

  void ConstantCache::update(STATE, Object* val, Module* mc, ConstantScope* sc) {
    value(state, val);
    scope(state, sc);
    under(state, mc);
    serial_ = state->shared().global_serial();
  }
}

