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

  class GlobalCache : public Lockable {
  public:
    struct CacheEntry {
      Module* klass;
      Symbol* name;
      Module* module;
      Executable* method;
      bool is_public;
      bool method_missing;
    };

    CacheEntry entries[CPU_CACHE_SIZE];
    thread::SpinLock lock_;

  public:

    static bool resolve(STATE, Symbol* name, Dispatch& msg, LookupData& lookup);
    bool resolve_i(STATE, Symbol* name, Dispatch& msg, LookupData& lookup);

    GlobalCache() {
      lock_.init();
      clear();
    }

    MethodCacheEntry* lookup_public(STATE, Module* mod, Class* cls, Symbol* name);
    MethodCacheEntry* lookup_private(STATE, Module* mod, Class* cls, Symbol* name);

    void clear(STATE, Symbol* name) {
      thread::SpinLock::LockGuard guard(lock_);
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

    void clear(STATE, Module* cls, Symbol* name) {
      CacheEntry* entry;
      thread::SpinLock::LockGuard guard(lock_);
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
      thread::SpinLock::LockGuard guard(lock_);
      retain_i(state, cls, name, mod, meth, missing, was_private);
    }

    private:

    void retain_i(STATE, Module* cls, Symbol* name, Module* mod, Executable* meth,
                bool missing, bool was_private = false) {
      CacheEntry* entry;

      entry = entries + CPU_CACHE_HASH(cls, name);
      entry->klass = cls;
      entry->name = name;
      entry->module = mod;
      entry->method_missing = missing;

      entry->method = meth;
      entry->is_public = !was_private;
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

    // Must be called with the lock held on +this+
    CacheEntry* lookup(STATE, Module* cls, Symbol* name) {
      CacheEntry* entry = entries + CPU_CACHE_HASH(cls, name);
      if(entry->name == name && entry->klass == cls) {
        return entry;
      }

      return NULL;
    }

  };
};

#endif
