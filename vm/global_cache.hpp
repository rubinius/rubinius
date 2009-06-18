#ifndef RBX_VM_GLOBAL_CACHE_HPP
#define RBX_VM_GLOBAL_CACHE_HPP

#include "vm/oop.hpp"
#include "vm/object_utils.hpp"

#include "builtin/compiledmethod.hpp"
#include "builtin/methodvisibility.hpp"

namespace rubinius {
  #define CPU_CACHE_SIZE 0x1000
  #define CPU_CACHE_MASK 0xfff
  #define CPU_CACHE_HASH(c,m) ((((uintptr_t)(c)>>3)^((uintptr_t)m)) & CPU_CACHE_MASK)

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

    void prune_young() {
      cache_entry* entry;
      for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
        entry = &entries[i];
        bool clear = false;

        Object* klass = reinterpret_cast<Object*>(entry->klass);
        if(!klass) continue;

        if(klass->young_object_p()) {
          if(klass->forwarded_p()) {
            entry->klass = (Module*)klass->forward();
          } else {
            clear = true;
          }
        }

        Object* mod = reinterpret_cast<Object*>(entry->module);
        if(mod->young_object_p()) {
          if(mod->forwarded_p()) {
            entry->module = (Module*)mod->forward();
          } else {
            clear = true;
          }
        }

        Object* exec = reinterpret_cast<Object*>(entry->method);
        if(exec->young_object_p()) {
          if(exec->forwarded_p()) {
            entry->method = (Executable*)exec->forward();
          } else {
            clear = true;
          }
        }

        if(clear) {
          entry->klass = 0;
          entry->name = 0;
          entry->module = 0;
          entry->is_public = true;
          entry->method_missing = false;
        }
      }
    }

    void prune_unmarked() {
      cache_entry* entry;
      for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
        entry = &entries[i];
        Object* klass = reinterpret_cast<Object*>(entry->klass);
        if(!klass) continue;

        Object* mod = reinterpret_cast<Object*>(entry->module);
        Object* exec = reinterpret_cast<Object*>(entry->method);

        if(!klass->marked_p() || !mod->marked_p() || !exec->marked_p()) {
          entry->klass = 0;
          entry->name = 0;
          entry->module = 0;
          entry->is_public = true;
          entry->method_missing = false;
        }
      }
    }

    void retain(STATE, Module* cls, Symbol* name, Module* mod, Executable* meth, bool missing) {
      struct cache_entry* entry;

      entry = entries + CPU_CACHE_HASH(cls, name);
      entry->klass = cls;
      entry->name = name;
      entry->module = mod;
      entry->method_missing = missing;

      if(kind_of<MethodVisibility>(meth)) {
        MethodVisibility* vis = as<MethodVisibility>(meth);
        entry->is_public = vis->public_p(state);
        entry->method = vis->method();
      } else {
        entry->method = meth;
        entry->is_public = true;
      }
    }
  };
};

#endif
