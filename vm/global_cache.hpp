#ifndef RBX_GLOBAL_CACHE_HPP
#define RBX_GLOBAL_CACHE_HPP

#include "objects.hpp"

namespace rubinius {
  #define CPU_CACHE_SIZE 0x1000
  #define CPU_CACHE_MASK 0xfff
  #define CPU_CACHE_HASH(c,m) ((((uintptr_t)(c)>>3)^((uintptr_t)m)) & CPU_CACHE_MASK)

  class GlobalCache {
  public:
    struct cache_entry {
      Module* klass;
      SYMBOL name;
      Module* module;
      OBJECT method;
      bool is_public;
    };

    struct cache_entry entries[CPU_CACHE_SIZE];

    GlobalCache() {
      for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
        entries[i].klass = 0;
        entries[i].name  = 0;
        entries[i].module = 0;
        entries[i].method= 0;
        entries[i].is_public = true;
      }
    }

    struct cache_entry* lookup(Module* cls, SYMBOL name) {
      struct cache_entry* entry;

      entry = entries + CPU_CACHE_HASH(cls, name);
      if(entry->name == name && entry->klass == cls) {
        return entry;
      }

      return NULL;
    }

    void retain(STATE, Module* cls, SYMBOL name, Module* mod, OBJECT meth) {
      struct cache_entry* entry;

      entry = entries + CPU_CACHE_HASH(cls, name);
      entry->klass = cls;
      entry->name = name;
      entry->module = mod;

      if(kind_of<MethodVisibility>(meth)) {
        MethodVisibility* vis;
        vis = as<MethodVisibility>(meth);

        if(vis->public_p(state)) {
          entry->is_public = true;
        } else {
          entry->is_public = false;
        }

        entry->method = vis->method;
      } else {
        entry->method = meth;
        entry->is_public = true;
      }
    }
  };
};

#endif
