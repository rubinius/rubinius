#ifndef RBX_VM_GLOBAL_CACHE_HPP
#define RBX_VM_GLOBAL_CACHE_HPP

#include "oop.hpp"
#include "object_utils.hpp"
#include "builtin/compiled_code.hpp"
#include "builtin/symbol.hpp"

#include "missing/unordered_set.hpp"

namespace rubinius {
  #define CPU_CACHE_SIZE 0x1000
  #define CPU_CACHE_MASK 0xfff
  #define CPU_CACHE_HASH(c,m) ((((uintptr_t)(c)>>3)^((uintptr_t)m)) & CPU_CACHE_MASK)

  struct LookupData;
  class Dispatch;

  typedef std_unordered_set<native_int> SeenMethodSet;

  class GlobalCache : public Lockable {
  public:
    struct CacheEntry {
      Module* klass;
      Module* module;
      Executable* method;
      Symbol* visibility;

      void clear() {
        klass = NULL;
        module = NULL;
        method = NULL;
        visibility = NULL;
      }
    };

    Symbol *entry_names[CPU_CACHE_SIZE];
    CacheEntry entries[CPU_CACHE_SIZE];
    SeenMethodSet seen_methods;
    utilities::thread::SpinLock lock_;

  public:

    static bool resolve(STATE, Symbol* name, Dispatch& msg, LookupData& lookup);
    bool resolve_i(STATE, Symbol* name, Dispatch& msg, LookupData& lookup);

    GlobalCache() {
      reset();
    }

    void reset() {
      lock_.init();
      clear();
    }

    void clear(STATE, Symbol* name) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
        if(entry_names[i] == name) {
          entry_names[i] = NULL;
          entries[i].clear();
        }
      }
    }

    void clear(STATE, Module* cls, Symbol* name) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      size_t i = CPU_CACHE_HASH(cls, name);
      if(entry_names[i] == name && entries[i].klass == cls) {
        entry_names[i] = NULL;
        entries[i].clear();
      }
    }

    void prune_young();

    bool has_seen(STATE, Symbol* sym);
    void add_seen(STATE, Symbol* sym);

    void prune_unmarked(unsigned int mark);

    void retain(STATE, Module* cls, Symbol* name, Module* mod, Executable* meth,
                Symbol* visibility) {
      utilities::thread::SpinLock::LockGuard guard(lock_);
      retain_i(state, cls, name, mod, meth, visibility);
    }

    private:

    void retain_i(STATE, Module* cls, Symbol* name, Module* mod, Executable* meth,
                  Symbol* visibility) {

      seen_methods.insert(name->index());

      CacheEntry* entry;

      entry = entries + CPU_CACHE_HASH(cls, name);
      entry_names[CPU_CACHE_HASH(cls, name)] = name;
      entry->klass = cls;
      entry->module = mod;

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
