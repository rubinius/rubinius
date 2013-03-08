#include "detection.hpp"

#ifdef OS_X_10_5
#ifndef RBX_HAVE_TR1_HASH
#include "missing/leopard_hashtable.hpp"
#endif
#endif

#include "inline_cache.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"
#include "global_cache.hpp"

#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/executable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/alias.hpp"
#include "builtin/call_unit.hpp"
#include "builtin/cache.hpp"

namespace rubinius {

  MethodMissingReason InlineCache::fill_public(STATE, Object* self, Symbol* name,
                                               Class* klass, MethodCacheEntry*& mce,
                                               bool super)
  {
    MethodTableBucket* entry;

    Module* module = klass;

    // Check the global cache first!
    mce = state->vm()->global_cache()->lookup_public(state, module, klass, name);

    if(mce) return eNone;

    bool skip_vis_check = false;

    MethodTableBucket* vis_entry = 0;

    Symbol* target_name = name;

    do {
      entry = module->method_table()->find_entry(state, target_name);

      /* Nothing, there? Ok, keep looking. */
      if(entry) {
        /* A 'false' method means to terminate method lookup.
         * (eg. undef_method) */
        if(entry->undef_p(state)) return eNormal;

        if(!skip_vis_check) {
          /* The method is private, but this wasn't a private send. */
          if(entry->private_p(state)) {
            return ePrivate;
          } else if(entry->protected_p(state)) {
            /* The method is protected, but it's not being called from
             * the same module */
            Module* check_mod;
            if(IncludedModule* im = try_as<IncludedModule>(module)) {
              check_mod = im->module();
            } else {
              check_mod = module;
            }

            if(!self->kind_of_p(state, check_mod)) {
              return eProtected;
            }
          }
        }

        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks.
         *
         * This is pretty much always where a subclass marks a superclass
         * method as public. We don't move the method, we just put this
         * marker into the method table. */
        if(entry->method()->nil_p()) {
          vis_entry = entry;
          skip_vis_check = true;
        } else {
          Module* use_module;
          Executable* use_exec;

          if(Alias* alias = try_as<Alias>(entry->method())) {
            // Same check as above, allow an alias to be for a superclass
            // method.
            if(alias->original_exec()->nil_p()) {
              vis_entry = entry;
              skip_vis_check = true;
              use_exec = 0;
              target_name = alias->original_name();
            } else {
              use_exec = alias->original_exec();
              use_module = alias->original_module();
            }
          } else {
            use_module = module;
            use_exec = entry->method();
          }

          if(use_exec) {
            mce = MethodCacheEntry::create(state, klass, use_module, use_exec, eNone, super);

            if(!vis_entry) vis_entry = entry;

            state->vm()->global_cache()->retain(state, klass, name, mce->stored_module(),
                  mce->method(), false,
                  vis_entry->visibility());

            return eNone;
          }
        }
      }

      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return eNormal;
    } while(1);

    // Shouldn't be here!
    rubinius::bug("Control flow bug in method lookup");
  }

  bool InlineCache::fill_private(STATE, Symbol* name, Module* start,
                                 Class* klass,
                                 MethodCacheEntry*& mce, bool super)
  {
    MethodTableBucket* entry;

    Module* module = start;

    // Check the global cache first!
    mce = state->vm()->global_cache()->lookup_private(state, start, klass, name);
    if(mce) return true;

    MethodTableBucket* vis_entry = 0;

    do {
      entry = module->method_table()->find_entry(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(entry) {

        /* A 'false' method means to terminate method lookup.
         * (eg. undef_method) */
        if(entry->undef_p(state)) return false;

        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks.
         *
         * This is pretty much always where a subclass marks a superclass
         * method as public. We don't move the method, we just put this
         * marker into the method table. */
        if(!entry->method()->nil_p()) {
          if(Alias* alias = try_as<Alias>(entry->method())) {
            mce = MethodCacheEntry::create(state, klass,
                                           alias->original_module(),
                                           alias->original_exec(),
                                           eNone, super);
          } else {
            mce = MethodCacheEntry::create(state, klass, module, entry->method(), eNone, super);
          }

          if(!vis_entry) vis_entry = entry;

          state->vm()->global_cache()->retain(state, start, name, mce->stored_module(),
                mce->method(), false,
                vis_entry->visibility());

          return true;
        } else {
          // Remember this entry as defining the visibility
          vis_entry = entry;
        }
      }

      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return false;
    } while(1);

    // Shouldn't be here!
    rubinius::bug("Control flow bug in method lookup");
  }

  bool InlineCache::fill_method_missing(STATE, Class* klass,
                                        MethodMissingReason reason,
                                        MethodCacheEntry*& mce)
  {
    MethodTableBucket* entry;

    Module* module = klass;

    Symbol* name = G(sym_method_missing);

    do {
      entry = module->method_table()->find_entry(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(entry) {

        /* A 'false' method means to terminate method lookup.
         * (eg. undef_method) */
        if(entry->undef_p(state)) return false;

        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks.
         *
         * This is pretty much always where a subclass marks a superclass
         * method as public. We don't move the method, we just put this
         * marker into the method table. */
        if(!entry->method()->nil_p()) {
          if(Alias* alias = try_as<Alias>(entry->method())) {
            mce = MethodCacheEntry::create(state, klass,
                                           alias->original_module(),
                                           alias->original_exec(),
                                           reason, reason == eSuper);
          } else {
            mce = MethodCacheEntry::create(state, klass,
                                           module,
                                           entry->method(),
                                           reason, reason == eSuper);
          }

          return true;
        }
      }

      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return false;
    } while(1);

    // Shouldn't be here!
    rubinius::bug("Control flow bug in method lookup");
  }

  MethodCacheEntry* InlineCache::update_and_validate(STATE, CallFrame* call_frame, Object* recv) {

    Class* const recv_class = recv->lookup_begin(state);
    MethodCacheEntry* mce = get_cache(recv_class);

    if(likely(mce)) return mce;

    MethodMissingReason reason =
      fill_public(state, call_frame->self(), name, recv_class, mce);
    if(reason != eNone) return 0;

    if(unlikely(growth_cache_size(mce->receiver_class_id()) > 0)) {
      execute_backend_ = check_cache_poly;
    }

    set_cache(mce);

    call_frame->compiled_code->write_barrier(state, mce);

    return mce;
  }

  MethodCacheEntry* InlineCache::update_and_validate_private(STATE, CallFrame* call_frame, Object* recv) {

    Class* const recv_class = recv->lookup_begin(state);
    MethodCacheEntry* mce = get_cache(recv_class);

    if(likely(mce)) return mce;

    if(!fill_private(state, name, recv_class, recv_class, mce)) return 0;

    if(unlikely(growth_cache_size(mce->receiver_class_id()) > 0)) {
      execute_backend_ = check_cache_poly;
    }

    set_cache(mce);
    call_frame->compiled_code->write_barrier(state, mce);

    return mce;
  }

  Object* InlineCache::empty_cache_custom(STATE, InlineCache* cache, CallFrame* call_frame,
                                          Arguments& args)
  {
    args.set_name(cache->name);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    Array*  ary = Array::create(state, args.total() + 2);
    ary->set(state, 0, recv);
    ary->set(state, 1, cache->name);

    for(size_t i = 0; i < args.total(); i++) {
      ary->set(state, i + 2, args.get_argument(i));
    }

    Object* ret = G(rubinius)->send(state, call_frame, state->symbol("bind_call"), ary);

    if(!ret) return 0;

    if(CallUnit* cu = try_as<CallUnit>(ret)) {
      MethodCacheEntry* mce = 0;

      mce = MethodCacheEntry::create(state, recv_class, cu->module(), cu->executable(), eNone, false);

      cache->set_cache(mce);

      call_frame->compiled_code->write_barrier(state, mce);

      cache->call_unit_ = cu;
      call_frame->compiled_code->write_barrier(state, cu);

      cache->execute_backend_ = check_cache_custom;

      return cu->execute(state, call_frame, cu, mce->method(), mce->stored_module(), args);
    } else {
      Exception::internal_error(state, call_frame, "bind_call must return CallUnit");
      return 0;
    }
  }

  Object* InlineCache::check_cache_custom(STATE, InlineCache* cache,
      CallFrame* call_frame, Arguments& args)
  {
    MethodCacheEntry* mce = cache->get_single_cache();

    args.set_name(cache->name);
    return cache->call_unit_->execute(state, call_frame, cache->call_unit_,
                                      mce->method(), mce->stored_module(), args);
  }



  Object* InlineCache::empty_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    args.set_name(cache->name);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    MethodCacheEntry* mce = NULL;

    MethodMissingReason reason =
      cache->fill_public(state, call_frame->self(), cache->name,
                         recv_class, mce);

    if(reason != eNone) {
      state->vm()->set_method_missing_reason(reason);

      if(!cache->fill_method_missing(state, recv_class, reason, mce)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name);
      if(unlikely(cache->growth_cache_size(mce->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(mce->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        if(recv->fixnum_p()) {
          cache->execute_backend_ = check_cache_fixnum;
        } else if(recv->symbol_p()) {
          cache->execute_backend_ = check_cache_symbol;
        } else if(recv->reference_p()) {
          cache->execute_backend_ = check_cache_reference;
        } else {
          cache->execute_backend_ = check_cache;
        }
      }
    }

    cache->set_cache(mce);

    call_frame->compiled_code->write_barrier(state, mce);

    Executable* meth = mce->method();
    Module* mod = mce->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::empty_cache_private(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    args.set_name(cache->name);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    MethodCacheEntry* mce = 0;

    if(!cache->fill_private(state, cache->name, recv_class, recv_class, mce)) {
      state->vm()->set_method_missing_reason(eNormal);

      if(!cache->fill_method_missing(state, recv_class, eNormal, mce)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name);
      if(unlikely(cache->growth_cache_size(mce->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(mce->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache;
      }
    }

    cache->set_cache(mce);

    call_frame->compiled_code->write_barrier(state, mce);

    Executable* meth = mce->method();
    Module* mod = mce->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::empty_cache_vcall(STATE, InlineCache* cache, CallFrame* call_frame,
                                         Arguments& args)
  {
    args.set_name(cache->name);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    MethodCacheEntry* mce = NULL;

    if(!cache->fill_private(state, cache->name, recv_class, recv_class, mce)) {
      state->vm()->set_method_missing_reason(eVCall);

      if(!cache->fill_method_missing(state, recv_class, eVCall, mce)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name);
      if(unlikely(cache->growth_cache_size(mce->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(mce->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache;
      }
    }

    cache->set_cache(mce);

    call_frame->compiled_code->write_barrier(state, mce);

    Executable* meth = mce->method();
    Module* mod = mce->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::empty_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Symbol* original_name = call_frame->original_name();
    if(cache->name != original_name) {
      cache->name = original_name;
    }

    args.set_name(cache->name);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    MethodCacheEntry* mce = 0;

    Module* const start = call_frame->module()->superclass();

    if(start->nil_p() || !cache->fill_private(state, cache->name, start, recv_class, mce, true)) {
      state->vm()->set_method_missing_reason(eSuper);

      // Don't use start when looking up method_missing!
      // Always completely redispatch for method_missing.
      // github#157
      if(!cache->fill_method_missing(state, recv_class, eSuper, mce)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name);

      if(unlikely(cache->growth_cache_size(mce->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache_super_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(mce->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache_super;
      }
    }

    cache->set_cache(mce);

    call_frame->compiled_code->write_barrier(state, mce);

    Executable* meth = mce->method();
    Module* mod = mce->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::check_cache_fixnum(STATE, InlineCache* cache,
      CallFrame* call_frame, Arguments& args)
  {
    args.set_name(cache->name);
    Object* const recv = args.recv();

    if(unlikely(!recv->fixnum_p())) {
      return cache->initialize(state, call_frame, args);
    }

    MethodCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(G(fixnum_class), entry);
    if(likely(ic)) {
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      ic->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_symbol(STATE, InlineCache* cache,
      CallFrame* call_frame, Arguments& args)
  {
    args.set_name(cache->name);
    Object* const recv = args.recv();

    if(unlikely(!recv->symbol_p())) {
      return cache->initialize(state, call_frame, args);
    }

    MethodCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(G(symbol), entry);

    if(likely(ic)) {
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      ic->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_reference(STATE, InlineCache* cache,
      CallFrame* call_frame, Arguments& args)
  {
    args.set_name(cache->name);
    Object* const recv = args.recv();

    if(unlikely(!recv->reference_p())) {
      return cache->initialize(state, call_frame, args);
    }

    MethodCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(recv->reference_class(), entry);

    if(likely(ic)) {
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      ic->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    args.set_name(cache->name);
    Class* const recv_class = args.recv()->lookup_begin(state);

    MethodCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(recv_class, entry);

    if(likely(ic)) {
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      ic->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {

    args.set_name(cache->name);
    Class* const recv_class = args.recv()->lookup_begin(state);

    MethodCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(recv_class, entry);

    if(likely(ic)) {
      args.unshift(state, cache->name);

      state->vm()->set_method_missing_reason(entry->method_missing());
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      ic->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Symbol* current_name = call_frame->original_name();
    args.set_name(cache->name);
    Class* const recv_class = args.recv()->lookup_begin(state);

    MethodCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(recv_class, entry);

    if(likely(ic && current_name == cache->name))
    {
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      ic->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    cache->name = current_name;

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_super_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Symbol* current_name = call_frame->original_name();
    args.set_name(cache->name);
    Class* const recv_class = args.recv()->lookup_begin(state);

    MethodCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(recv_class, entry);

    if(likely(ic))
    {
      args.unshift(state, cache->name);

      state->vm()->set_method_missing_reason(entry->method_missing());
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      ic->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    cache->name = current_name;

    return cache->initialize(state, call_frame, args);
  }


  Object* InlineCache::check_cache_poly(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    args.set_name(cache->name);
    Class* const recv_class = args.recv()->lookup_begin(state);

    MethodCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(recv_class, entry);

    if(likely(ic)) {
      if(entry->method_missing() != eNone) {
        args.unshift(state, cache->name);
      }
      if(entry->super()) {
        Symbol* current_name = call_frame->original_name();
        if(current_name != cache->name) {
          cache->name = current_name;
          return cache->initialize(state, call_frame, args);
        }
      }

      state->vm()->set_method_missing_reason(entry->method_missing());
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      ic->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  void InlineCache::print_location(STATE, std::ostream& stream) {
#ifdef TRACK_IC_LOCATION
    stream << vmm_->original->file()->c_str(state)
           << ":" << vmm_->original->line(state, ip_);
#else
    stream << "(unknown)";
#endif
  }

  void InlineCache::print(STATE, std::ostream& stream) {
    stream << "name: " << name->debug_str(state) << "\n"
           << "seen classes: " << classes_seen() << "\n"
           << "overflows: " << seen_classes_overflow_ << "\n"
           << "classes:\n";

    for(int i = 0; i < cTrackedICHits; i++) {
      if(cache_[i].entry()) {
        Module* mod = cache_[i].entry()->receiver_class();
        if(mod) {
          if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
            if(Module* inner = try_as<Module>(sc->attached_instance())) {
              stream << "  SingletonClass:" << inner->debug_str(state);
            } else {
              stream << "  SingletonClass:" << sc->attached_instance()->class_object(state)->debug_str(state);
            }
          } else {
            stream << "  " << mod->debug_str(state);
          }

          stream << "\n";
        }
      }
    }
  }
}
