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
      Module* module;
      Executable* method;
      Symbol* visibility;
      bool method_missing;

      void clear() {
        klass = NULL;
        module = NULL;
        method = NULL;
        visibility = NULL;
        method_missing = false;
      }
    };

    Symbol *entry_names[CPU_CACHE_SIZE];
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
        if(entry_names[i] == name) {
          entry_names[i] = NULL;
          entries[i].clear();
        }
      }
    }

    void clear(STATE, Module* cls, Symbol* name) {
      thread::SpinLock::LockGuard guard(lock_);
      size_t i = CPU_CACHE_HASH(cls, name);
      if(entry_names[i] == name && entries[i].klass == cls) {
        entry_names[i] = NULL;
        entries[i].clear();
      }
    }

    void prune_young();

    void prune_unmarked(int mark);

    void retain(STATE, Module* cls, Symbol* name, Module* mod, Executable* meth,
                bool missing, Symbol* visibility) {
      thread::SpinLock::LockGuard guard(lock_);
      retain_i(state, cls, name, mod, meth, missing, visibility);
    }

    private:

    void retain_i(STATE, Module* cls, Symbol* name, Module* mod, Executable* meth,
                bool missing, Symbol* visibility) {
      CacheEntry* entry;

      entry = entries + CPU_CACHE_HASH(cls, name);
      entry_names[CPU_CACHE_HASH(cls, name)] = name;
      entry->klass = cls;
      entry->module = mod;
      entry->method_missing = missing;

      entry->method = meth;
      entry->visibility = visibility;
    }

    void clear() {
      for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
        entry_names[i] = NULL;
        entries[i].clear();
      }
    }

    // Must be called with the lock held on +this+
    CacheEntry* lookup(STATE, Module* cls, Symbol* name) {
      Symbol* entry_name = entry_names[CPU_CACHE_HASH(cls, name)];
      CacheEntry* entry = entries + CPU_CACHE_HASH(cls, name);
      if(entry_name == name && entry->klass == cls) {
        return entry;
      }

      return NULL;
    }

  };
};

#endif
