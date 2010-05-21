#ifndef RBX_VM_GLOBAL_CACHE_HPP
#define RBX_VM_GLOBAL_CACHE_HPP

#include "vm/oop.hpp"
#include "vm/object_utils.hpp"

#include "builtin/compiledmethod.hpp"

namespace rubinius {
  #define CPU_CACHE_SIZE 0x1000
  #define CPU_CACHE_MASK 0xfff
  #define CPU_CACHE_HASH(c,m) ((((uintptr_t)(c)>>3)^((uintptr_t)m)) & CPU_CACHE_MASK)

  struct LookupData;
  class Dispatch;

  class GlobalCache {
  public:
    struct cache_entry {
      Module* klass;
      Symbol* name;
      Module* module;
      Executable* method;
      bool is_public;
      bool method_missing;
    };

    struct cache_entry entries[CPU_CACHE_SIZE];

  public:

    static bool resolve(STATE, Symbol* name, Dispatch& msg, LookupData& lookup);

    GlobalCache() {
      clear();
    }

    /** @todo This does NOT handle null? --rue */
    struct cache_entry* lookup(Module* cls, Symbol* name) {
      struct cache_entry* entry;

      entry = entries + CPU_CACHE_HASH(cls, name);
      if(entry->name == name && entry->klass == cls) {
        return entry;
      }

      return NULL;
    }

    void clear() {
      for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
        entries[i].klass = 0;
        entries[i].name  = 0;
        entries[i].module = 0;
        entries[i].method = 0;
        entries[i].is_public = true;
        entries[i].method_missing = false;
      }
    }

    void clear(Symbol* name) {
      for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
        if(entries[i].name == name) {
          entries[i].klass = NULL;
          entries[i].name = NULL;
          entries[i].module = NULL;
          entries[i].method = NULL;
          entries[i].method_missing = false;
        }
      }
    }

    void clear(Module* cls, Symbol* name) {
      struct cache_entry* entry;

      entry = entries + CPU_CACHE_HASH(cls, name);
      if(entry->name == name && entry->klass == cls) {
        entry->klass = NULL;
        entry->name = NULL;
        entry->module = NULL;
        entry->method = NULL;
        entry->method_missing = false;
      }
    }

    void prune_young();

    void prune_unmarked(int mark);

    void retain(STATE, Module* cls, Symbol* name, Module* mod, Executable* meth,
                bool missing, bool was_private = false) {
      struct cache_entry* entry;

      entry = entries + CPU_CACHE_HASH(cls, name);
      entry->klass = cls;
      entry->name = name;
      entry->module = mod;
      entry->method_missing = missing;

      entry->method = meth;
      entry->is_public = !was_private;
    }
  };
};

#endif
