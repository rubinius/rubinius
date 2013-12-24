#include "builtin/constant_cache.hpp"
#include "builtin/class.hpp"
#include "builtin/constant_scope.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"

namespace rubinius {
  void ConstantCache::init(STATE) {
    GO(constant_cache).set(
        ontology::new_class(state, "ConstantCache",
          G(object), G(rubinius)));

  }

  ConstantCache* ConstantCache::create(STATE, ConstantCache* existing, Object* value, Module* under,
                                             ConstantScope* scope) {
    ConstantCache* cache = state->new_object_dirty<ConstantCache>(G(constant_cache));

    cache->name(state, existing->name());
    cache->executable(state, existing->executable());
    cache->ip_ = existing->ip();
    cache->value(state, value);
    cache->under(state, under);
    cache->scope(state, scope);
    cache->serial_ = state->shared().global_serial();
    return cache;
  }

  ConstantCache* ConstantCache::create(STATE, ConstantCache* existing, Object* value,
                                             ConstantScope* scope) {
    return create(state, existing, value, nil<Module>(), scope);
  }

  ConstantCache* ConstantCache::empty(STATE, Symbol* name, Executable* executable, int ip) {
    ConstantCache* cache = state->new_object_dirty<ConstantCache>(G(constant_cache));

    cache->name(state, name);
    cache->executable(state, executable);
    cache->ip_ = ip;
    cache->value(state, cNil);
    cache->under(state, nil<Module>());
    cache->scope(state, nil<ConstantScope>());
    cache->serial_ = -1;
    return cache;
  }

  Integer* ConstantCache::ip_prim(STATE) {
    return Integer::from(state, ip_);
  }

  Integer* ConstantCache::serial_prim(STATE) {
    return Integer::from(state, serial_);
  }
}

