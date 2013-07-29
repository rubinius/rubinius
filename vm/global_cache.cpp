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

namespace rubinius {
  static bool hierarchy_resolve(STATE, Symbol* name, Dispatch& msg, LookupData& lookup) {
    Module* module = lookup.from;
    MethodTableBucket* entry;
    bool skip_vis_check = false;

    do {

      if(module != module->origin()) {
        module = module->superclass();
      }

      entry = module->method_table()->find_entry(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(!entry) goto keep_looking;

      /* A 'false' method means to terminate method lookup.
       * (eg. undef_method) */
      if(entry->undef_p(state)) {
        msg.method_missing = eNormal;
        return false;
      }

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
          msg.visibility = entry->visibility();
        }
        if(entry->method()->nil_p()) {
          skip_vis_check = true;
          goto keep_looking;
        }

        if(Alias* alias = try_as<Alias>(entry->method())) {
          if(alias->original_exec()->nil_p()) {
            name           = alias->original_name();
            msg.visibility = entry->visibility();
            skip_vis_check = true;
          } else {
            msg.method = alias->original_exec();
            msg.module = alias->original_module();
          }
        } else {
          msg.method = entry->method();
          msg.module = module;
        }
        if(msg.method) return true;
      } else {
        /* The method is private, but this wasn't a private send. */
        if(entry->private_p(state)) {
          msg.method_missing = ePrivate;
          return false;
        } else if(entry->protected_p(state)) {
          /* The method is protected, but it's not being called from
           * the same module, or we only want public methods. */
          Module* check_mod;
          if(IncludedModule* im = try_as<IncludedModule>(module)) {
            check_mod = im->module();
          } else {
            check_mod = module;
          }

          if(lookup.min_visibility == G(sym_public) && !lookup.recv->kind_of_p(state, check_mod)) {
            msg.method_missing = eProtected;
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
          msg.visibility = entry->visibility();
        }
        if(entry->method()->nil_p()) {
          skip_vis_check = true;
          goto keep_looking;
        }

        if(Alias* alias = try_as<Alias>(entry->method())) {
          if(alias->original_exec()->nil_p()) {
            name           = alias->original_name();
            msg.visibility = entry->visibility();
            skip_vis_check = true;
          } else {
            msg.method = alias->original_exec();
            msg.module = alias->original_module();
          }
        } else {
          msg.method = entry->method();
          msg.module = module;
        }
        if(msg.method) return true;
      }

keep_looking:
      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) {
        msg.method_missing = eNormal;
        return false;
      }
    } while(1);

    rubinius::bug("Control flow bug in method lookup");
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
        msg.visibility = entry->visibility;

        return true;
      }
    }

    if(hierarchy_resolve(state, name, msg, lookup)) {
      retain_i(state, klass, name,
          msg.module, msg.method, msg.visibility);

      return true;
    }

    return false;
  }

  bool GlobalCache::has_seen(STATE, Symbol* sym) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    SeenMethodSet::const_iterator seen = seen_methods.find(sym->index());
    return seen != seen_methods.end();
  }

  void GlobalCache::add_seen(STATE, Symbol* sym) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    seen_methods.insert(sym->index());
  }

  void GlobalCache::prune_young() {
    for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
      CacheEntry* entry = &entries[i];
      bool clear = false;

      Object* klass = entry->klass;
      if(!klass) continue;

      if(klass->young_object_p()) {
        if(klass->forwarded_p()) {
          Module* fwd = force_as<Module>(klass->forward());
          entry->klass = fwd;
        } else {
          clear = true;
        }
      }

      Object* mod = entry->module;
      if(mod->young_object_p()) {
        if(mod->forwarded_p()) {
          entry->module = force_as<Module>(mod->forward());
        } else {
          clear = true;
        }
      }

      Object* exec = entry->method;
      if(exec->young_object_p()) {
        if(exec->forwarded_p()) {
          entry->method = force_as<Executable>(exec->forward());
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

  void GlobalCache::prune_unmarked(unsigned int mark) {
    for(size_t i = 0; i < CPU_CACHE_SIZE; i++) {
      CacheEntry* entry = &entries[i];
      Object* klass = entry->klass;
      if(!klass) continue;

      Object* mod = entry->module;
      Object* exec = entry->method;

      if(!klass->marked_p(mark) || !mod->marked_p(mark) || !exec->marked_p(mark)) {
        entry_names[i] = NULL;
        entry->clear();
      }
    }
  }
}
