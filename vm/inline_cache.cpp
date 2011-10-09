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
                                               Class* klass, MethodCacheEntry*& mce)
  {
    MethodTableBucket* entry;

    Module* module = klass;

    // Check the global cache first!
    GlobalCache::cache_entry* global_entry =
      state->global_cache()->lookup(state, module, name);

    if(global_entry &&
         global_entry->is_public &&
        !global_entry->method_missing) {

      mce = MethodCacheEntry::create(state, klass, global_entry->module,
                                     global_entry->method);

      return eNone;
    }

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
            mce = MethodCacheEntry::create(state, klass, use_module, use_exec);

            if(!vis_entry) vis_entry = entry;

            state->global_cache()->retain(state, klass, name, mce->stored_module(),
                  mce->method(), false,
                  !vis_entry->public_p(state));

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
                                 MethodCacheEntry*& mce)
  {
    MethodTableBucket* entry;

    Module* module = start;

    // Check the global cache first!
    GlobalCache::cache_entry* global_entry =
      state->global_cache()->lookup(state, module, name);

    if(global_entry && !global_entry->method_missing) {
      mce = MethodCacheEntry::create(state, klass, global_entry->module,
                                     global_entry->method);
      return true;
    }

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
                                           alias->original_exec());
          } else {
            mce = MethodCacheEntry::create(state, klass, module, entry->method());
          }

          if(!vis_entry) vis_entry = entry;

          state->global_cache()->retain(state, start, name, mce->stored_module(),
                mce->method(), false,
                !vis_entry->public_p(state));

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
                                           alias->original_exec());
          } else {
            mce = MethodCacheEntry::create(state, klass,
                                           module,
                                           entry->method());
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
    MethodCacheEntry* mce = cache_;

    Class* const recv_class = recv->lookup_begin(state);

    if(likely(mce && mce->receiver_class() == recv_class)) return mce;

    MethodMissingReason reason =
      fill_public(state, call_frame->self(), name, recv_class, mce);
    if(reason != eNone) return 0;

    update_seen_classes(mce);
    call_frame->cm->write_barrier(state, mce);

    return mce;
  }

  MethodCacheEntry* InlineCache::update_and_validate_private(STATE, CallFrame* call_frame, Object* recv) {
    MethodCacheEntry* mce = cache_;

    Class* const recv_class = recv->lookup_begin(state);

    if(likely(mce && mce->receiver_class() == recv_class)) return mce;

    if(!fill_private(state, name, recv_class, recv_class, mce)) return 0;

    update_seen_classes(mce);
    call_frame->cm->write_barrier(state, mce);

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

    Object* ret = G(rubinius)->send(state, call_frame, state->symbol("bind_call"),
                                    ary, Qnil, true);

    if(!ret) return 0;

    if(CallUnit* cu = try_as<CallUnit>(ret)) {
      MethodCacheEntry* mce = 0;

      mce = MethodCacheEntry::create(state, recv_class, cu->module(), cu->executable());

      atomic::memory_barrier();

      cache->cache_ = mce;
      call_frame->cm->write_barrier(state, mce);

      cache->call_unit_ = cu;
      call_frame->cm->write_barrier(state, cu);

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
    MethodCacheEntry* mce = cache->cache_;

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

    MethodCacheEntry* mce = 0;

    MethodMissingReason reason =
      cache->fill_public(state, call_frame->self(), cache->name,
                         recv_class, mce);

    if(reason != eNone) {
      state->set_method_missing_reason(reason);

      if(!cache->fill_method_missing(state, recv_class, mce)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
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

    // Make sure we sync here, so the MethodCacheEntry mce is
    // guaranteed completely initialized. Otherwise another thread
    // might see an incompletely initialized MethodCacheEntry.
    atomic::memory_barrier();

    cache->cache_ = mce;
    cache->update_seen_classes(mce);

    call_frame->cm->write_barrier(state, mce);

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
      state->set_method_missing_reason(eNormal);

      if(!cache->fill_method_missing(state, recv_class, mce)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
    } else {
      cache->execute_backend_ = check_cache;
    }

    // Make sure we sync here, so the MethodCacheEntry mce is
    // guaranteed completely initialized. Otherwise another thread
    // might see an incompletely initialized MethodCacheEntry.
    atomic::memory_barrier();

    cache->cache_ = mce;
    cache->update_seen_classes(mce);

    call_frame->cm->write_barrier(state, mce);

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

    MethodCacheEntry* mce = 0;

    if(!cache->fill_private(state, cache->name, recv_class, recv_class, mce)) {
      state->set_method_missing_reason(eVCall);

      if(!cache->fill_method_missing(state, recv_class, mce)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
    } else {
      cache->execute_backend_ = check_cache;
    }

    // Make sure we sync here, so the MethodCacheEntry mce is
    // guaranteed completely initialized. Otherwise another thread
    // might see an incompletely initialized MethodCacheEntry.
    atomic::memory_barrier();

    cache->cache_ = mce;
    cache->update_seen_classes(mce);

    call_frame->cm->write_barrier(state, mce);

    Executable* meth = mce->method();
    Module* mod = mce->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::empty_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    args.set_name(cache->name);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    MethodCacheEntry* mce = 0;

    Module* const start = call_frame->module()->superclass();

    if(start->nil_p() || !cache->fill_private(state, cache->name, start, recv_class, mce)) {
      state->set_method_missing_reason(eSuper);

      // Don't use start when looking up method_missing!
      // Always completely redispatch for method_missing.
      // github#157
      if(!cache->fill_method_missing(state, recv_class, mce)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        cache->private_lock_ = 0;
        return 0;
      }

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
    } else {
      cache->execute_backend_ = check_cache;
    }

    // Make sure we sync here, so the MethodCacheEntry mce is
    // guaranteed completely initialized. Otherwise another thread
    // might see an incompletely initialized MethodCacheEntry.
    atomic::memory_barrier();

    cache->cache_ = mce;
    cache->update_seen_classes(mce);

    call_frame->cm->write_barrier(state, mce);

    Executable* meth = mce->method();
    Module* mod = mce->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::check_cache_fixnum(STATE, InlineCache* cache,
      CallFrame* call_frame, Arguments& args)
  {
    MethodCacheEntry* mce = cache->cache_;

    args.set_name(cache->name);
    if(likely(mce && args.recv()->fixnum_p())) {

      Executable* meth = mce->method();
      Module* mod = mce->stored_module();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_symbol(STATE, InlineCache* cache,
      CallFrame* call_frame, Arguments& args)
  {
    MethodCacheEntry* mce = cache->cache_;

    args.set_name(cache->name);
    if(likely(mce && args.recv()->symbol_p())) {

      Executable* meth = mce->method();
      Module* mod = mce->stored_module();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_reference(STATE, InlineCache* cache,
      CallFrame* call_frame, Arguments& args)
  {
    MethodCacheEntry* mce = cache->cache_;

    args.set_name(cache->name);
    Object* const recv = args.recv();

    if(likely(mce && recv->reference_p() &&
              recv->reference_class() == mce->receiver_class())) {

      Executable* meth = mce->method();
      Module* mod = mce->stored_module();

      // if(mce->execute) {
        // return mce->execute(state, call_frame, meth, mod, args);
      // }

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    MethodCacheEntry* mce = cache->cache_;

    args.set_name(cache->name);
    if(likely(mce && mce->receiver_class() == args.recv()->lookup_begin(state))) {

      Executable* meth = mce->method();
      Module* mod = mce->stored_module();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    MethodCacheEntry* mce = cache->cache_;

    args.set_name(cache->name);
    if(likely(mce && mce->receiver_class() == args.recv()->lookup_begin(state))) {

      args.unshift(state, cache->name);

      Executable* meth = mce->method();
      Module* mod = mce->stored_module();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  void InlineCache::update_seen_classes(MethodCacheEntry* mce) {

    for(int i = 0; i < cTrackedICHits; i++) {
      Module* mod = seen_classes_[i].klass();
      if(mod == mce->receiver_class()) {
        return;
      }
    }

    // This comparison is potentially thread unsafe,
    // but since we never decrease seen_classes_overflow_,
    // it's not possible to cause any issues here
    if( seen_classes_overflow_ < cTrackedICHits - 1) {
      // Ok, we've arrived here and found no hit. There's
      // potentially room for registering another class,
      // so we should lock it here before we're allowed to write it.

      while(!atomic::compare_and_swap(&private_lock_, 0, 1));

      for(int i = 0; i < cTrackedICHits; i++) {
        if(!seen_classes_[i].klass()) {
          // An empty space, record it.
          seen_classes_[i].assign(mce->receiver_class());
          private_lock_ = 0;
          return;
        }
      }
    }

    // Hmmm, what do we do when this is full? Just ignore them?
    // For now, just keep track of how many times we overflow.
    seen_classes_overflow_++;
    private_lock_ = 0;
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
      Module* mod = seen_classes_[i].klass();
      if(mod) {
        if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
          if(Module* inner = try_as<Module>(sc->attached_instance())) {
            stream << "  SingletonClass:" << inner->name()->debug_str(state);
          } else {
            stream << "  SingletonClass:" << sc->attached_instance()->class_object(state)->name()->debug_str(state);
          }
        } else {
          stream << "  " << mod->name()->debug_str(state);
        }

        stream << "\n";
      }
    }
  }

  void InlineCacheRegistry::add_cache(STATE, Symbol* sym, InlineCache* cache) {
    SYNC(state);
    // TODO make sure cache isn't already known?
    caches_[sym->index()].push_back(cache);
  }

  void InlineCacheRegistry::remove_cache(STATE, Symbol* sym, InlineCache* cache) {
    SYNC(state);
    CacheVector& vec = caches_[sym->index()];
    for(CacheVector::iterator i = vec.begin();
        i != vec.end();
        ++i) {
      if(*i == cache) {
        vec.erase(i);
        return;
      }
    }
  }

  void InlineCacheRegistry::clear(STATE, Symbol* sym) {
    SYNC(state);
    CacheVector& vec = caches_[sym->index()];

    for(CacheVector::iterator i = vec.begin();
        i != vec.end();
        ++i) {
      (*i)->clear();
    }
  }

  static inline double ratio(int part, int total) {
    return 100 * ((double)part / (double)total);
  }

  void InlineCacheRegistry::print_stats(STATE) {
    SYNC(state);
    int total = 0;
    std::vector<int> sizes(cTrackedICHits + 1);

    int overflow = 0;

    for(CacheHash::iterator hi = caches_.begin();
        hi != caches_.end();
        ++hi) {
      for(CacheVector::iterator vi = hi->second.begin();
          vi != hi->second.end();
          ++vi) {
        InlineCache* ic = *vi;
        int seen = ic->classes_seen();
        if(ic->seen_classes_overflow() > 0) {
          total++;
          overflow++;
        } else if(seen > 0) {
          total++;
          sizes[seen]++;
        }
      }
    }

    std::cerr << "IC Stats:\n";
    for(int i = 1; i < cTrackedICHits + 1; i++) {
      std::cerr << " " << i << ": " << sizes[i] << " "
                << ratio(sizes[i], total) << "%\n";
    }

    std::cerr << cTrackedICHits << "+: " << overflow << " "
              << ratio(overflow, total) << "%\n";

    // print out the mega-morphic ones
    std::cerr << "\nMegamorphic call sites:\n";

    for(CacheHash::iterator hi = caches_.begin();
        hi != caches_.end();
        ++hi) {
      for(CacheVector::iterator vi = hi->second.begin();
          vi != hi->second.end();
          ++vi) {
        InlineCache* ic = *vi;
        if(ic->seen_classes_overflow() > 0) {
          ic->print(state, std::cerr);
          std::cerr << "location: ";
          ic->print_location(state, std::cerr);
          std::cerr << "\n\n";
        }
      }
    }
  }
}
