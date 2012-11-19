#include "vm.hpp"
#include "global_cache.hpp"
#include "object_position.hpp"
#include "objectmemory.hpp"
#include "lookup_data.hpp"
#include "dispatch.hpp"

#include "builtin/symbol.hpp"
#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/object.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/alias.hpp"
#include "builtin/cache.hpp"

namespace rubinius {
  static bool hierarchy_resolve(STATE, Symbol* name, Dispatch& msg, LookupData& lookup,
                                  Symbol** visibility)
  {
    Module* module = lookup.from;
    MethodTableBucket* entry;
    bool skip_vis_check = false;

    do {
      entry = module->method_table()->find_entry(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(!entry) goto keep_looking;

      /* A 'false' method means to terminate method lookup.
       * (eg. undef_method) */
      if(entry->undef_p(state)) return false;

      /* If this was a private send, then we can handle use
       * any method seen. */
      if(lookup.min_visibility == G(sym_private) || skip_vis_check) {
        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks. If we are skipping visibility checks we
         * shouldn't update visibility anymore because the implementation
         * might have a different visibility than the original lookup.
         */
        if(!skip_vis_check) {
          *visibility = entry->visibility();
        }
        if(entry->method()->nil_p()) {
          skip_vis_check = true;
          goto keep_looking;
        }

        if(Alias* alias = try_as<Alias>(entry->method())) {
          msg.method = alias->original_exec();
          msg.module = alias->original_module();
        } else {
          msg.method = entry->method();
          msg.module = module;
        }
        break;
      } else {
        /* The method is private, but this wasn't a private send. */
        if(entry->private_p(state)) {
          return false;
        } else if(entry->protected_p(state)) {
          /* The method is protected, but it's not being called from
           * the same module, or we only want public methods. */
          if(lookup.min_visibility == G(sym_public) || !lookup.recv->kind_of_p(state, module)) {
            return false;
          }
        }

        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks. If we are skipping visibility checks we
         * shouldn't update visibility anymore because the implementation
         * might have a different visibility than the original lookup.
         */
        if(!skip_vis_check) {
          *visibility = entry->visibility();
        }
        if(entry->method()->nil_p()) {
          skip_vis_check = true;
          goto keep_looking;
        }

        if(Alias* alias = try_as<Alias>(entry->method())) {
          msg.method = alias->original_exec();
          msg.module = alias->original_module();
        } else {
          msg.method = entry->method();
          msg.module = module;
        }
        break;
      }

keep_looking:
      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return false;
    } while(1);

    return true;
  }

  MethodCacheEntry* GlobalCache::lookup_public(STATE, Module* mod, Class* cls, Symbol* name) {
    utilities::thread::SpinLock::LockGuard guard(lock_);

    Symbol* entry_name = entry_names[CPU_CACHE_HASH(mod, name)];
    CacheEntry* entry = entries + CPU_CACHE_HASH(mod, name);
    if(entry_name == name &&
         entry->klass == mod &&
         entry->visibility != G(sym_private) &&
        !entry->method_missing) {

      return MethodCacheEntry::create(state, cls, entry->module,
                                      entry->method, eNone, false);
    }

    return NULL;
  }

  MethodCacheEntry* GlobalCache::lookup_private(STATE, Module* mod, Class* cls, Symbol* name) {
    utilities::thread::SpinLock::LockGuard guard(lock_);

    Symbol* entry_name = entry_names[CPU_CACHE_HASH(mod, name)];
    CacheEntry* entry = entries + CPU_CACHE_HASH(mod, name);
    if(entry_name == name &&
         entry->klass == mod &&
        !entry->method_missing) {

      return MethodCacheEntry::create(state, cls, entry->module,
                                      entry->method, eNone, false);
    }

    return NULL;
  }

  bool GlobalCache::resolve(STATE, Symbol* name, Dispatch& msg, LookupData& lookup) {
    return state->vm()->global_cache()->resolve_i(state, name, msg, lookup);
  }

  bool GlobalCache::resolve_i(STATE, Symbol* name, Dispatch& msg, LookupData& lookup) {
    utilities::thread::SpinLock::LockGuard guard(lock_);

    Module* klass = lookup.from;
    CacheEntry* entry = this->lookup(state, klass, name);

    if(entry) {
      if(lookup.min_visibility == G(sym_private) ||
         entry->visibility     == G(sym_public)  ||
         lookup.min_visibility == entry->visibility) {
        msg.method = entry->method;
        msg.module = entry->module;
        msg.method_missing = entry->method_missing;

        return true;
      }
    }

    Symbol* visibility = G(sym_protected);
    if(hierarchy_resolve(state, name, msg, lookup, &visibility)) {
      retain_i(state, klass, name,
          msg.module, msg.method, msg.method_missing, visibility);

      return true;
    }

    return false;
  }

  void GlobalCache::prune_young() {
    CacheEntry* entry;

    for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
      entry = &entries[i];
      bool clear = false;

      Object* klass = reinterpret_cast<Object*>(entry->klass);
      if(!klass) continue;

      if(klass->young_object_p()) {
        if(klass->forwarded_p()) {
          Module* fwd = reinterpret_cast<Module*>(klass->forward());
          entry->klass = fwd;
        } else {
          clear = true;
        }
      }

      Object* mod = reinterpret_cast<Object*>(entry->module);
      if(mod->young_object_p()) {
        if(mod->forwarded_p()) {
          entry->module = reinterpret_cast<Module*>(mod->forward());
        } else {
          clear = true;
        }
      }

      Object* exec = reinterpret_cast<Object*>(entry->method);
      if(exec->young_object_p()) {
        if(exec->forwarded_p()) {
          entry->method = reinterpret_cast<Executable*>(exec->forward());
        } else {
          clear = true;
        }
      }

      if(clear) {
        entry_names[i] = NULL;
        entry->clear();
      }
    }
  }

  void GlobalCache::prune_unmarked(int mark) {
    CacheEntry* entry;

    for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
      entry = &entries[i];
      Object* klass = reinterpret_cast<Object*>(entry->klass);
      if(!klass) continue;

      Object* mod = reinterpret_cast<Object*>(entry->module);
      Object* exec = reinterpret_cast<Object*>(entry->method);

      if(!klass->marked_p(mark) || !mod->marked_p(mark) || !exec->marked_p(mark)) {
        entry_names[i] = NULL;
        entry->clear();
      }
    }
  }
}
