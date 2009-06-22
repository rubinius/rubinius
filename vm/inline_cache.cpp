#include "inline_cache.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"
#include "global_cache.hpp"

#include "builtin/symbol.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/executable.hpp"
#include "builtin/methodvisibility.hpp"

namespace rubinius {

  bool InlineCache::fill_public(STATE, Object* self, Symbol* name) {
    Object* entry;
    MethodVisibility* vis;

    Module* module = klass_;

    // Check the global cache first!
    GlobalCache::cache_entry* global_entry =
      state->global_cache->lookup(module, name);

    if(global_entry &&
        global_entry->is_public &&
        !global_entry->method_missing) {
      this->method = global_entry->method;
      this->module = global_entry->module;

      return true;
    }

    bool skip_vis_check = false;

    do {
      entry = module->method_table()->fetch(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(!entry->nil_p()) {

        /* A 'false' method means to terminate method lookup.
         * (eg. undef_method) */
        if(entry == Qfalse) return false;

        vis = try_as<MethodVisibility>(entry);

        if(!vis) {
          this->method = as<Executable>(entry);
          this->module = module;

          state->global_cache->retain(state, klass_, name, this->module,
                                      this->method, false, false);
          return true;
        }

        if(!skip_vis_check) {
          /* The method is private, but this wasn't a private send. */
          if(vis->private_p(state)) {
            return false;
          } else if(vis->protected_p(state)) {
            /* The method is protected, but it's not being called from
             * the same module */
            if(!self->kind_of_p(state, module)) {
              return false;
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
        if(vis->method()->nil_p()) {
          skip_vis_check = true;
        } else {
          this->method = as<Executable>(vis->method());
          this->module = module;

          state->global_cache->retain(state, klass_, name, this->module,
                                      this->method, false,
                                      !vis->public_p(state));
          return true;
        }
      }

      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return false;
    } while(1);

    // Shouldn't be here!
    rubinius::abort();
  }

  bool InlineCache::fill_private(STATE, Symbol* name, Module* start) {
    Object* entry;
    MethodVisibility* vis;

    Module* module = start;

    // Check the global cache first!
    GlobalCache::cache_entry* global_entry =
      state->global_cache->lookup(module, name);

    if(global_entry && !global_entry->method_missing) {
      this->method = global_entry->method;
      this->module = global_entry->module;

      return true;
    }

    do {
      entry = module->method_table()->fetch(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(!entry->nil_p()) {

        /* A 'false' method means to terminate method lookup.
         * (eg. undef_method) */
        if(entry == Qfalse) return false;

        vis = try_as<MethodVisibility>(entry);

        if(vis) {
          /* The method was callable, but we need to keep looking
           * for the implementation, so make the invocation bypass all further
           * visibility checks.
           *
           * This is pretty much always where a subclass marks a superclass
           * method as public. We don't move the method, we just put this
           * marker into the method table. */
          if(!vis->method()->nil_p()) {
            this->method = as<Executable>(vis->method());
            this->module = module;
            state->global_cache->retain(state, start, name, this->module,
                                        this->method, false,
                                        !vis->public_p(state));

            return true;
          }
        } else {
          this->method = as<Executable>(entry);
          this->module = module;

          state->global_cache->retain(state, start, name, this->module,
                                      this->method, false, false);

          return true;
        }
      }

      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return false;
    } while(1);

    // Shouldn't be here!
    rubinius::abort();
  }

  bool InlineCache::fill_method_missing(STATE, Module* module) {
    Object* entry;
    MethodVisibility* vis;

    Symbol* name = G(sym_method_missing);

    do {
      entry = module->method_table()->fetch(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(!entry->nil_p()) {

        /* A 'false' method means to terminate method lookup.
         * (eg. undef_method) */
        if(entry == Qfalse) return false;

        vis = try_as<MethodVisibility>(entry);

        if(vis) {
          /* The method was callable, but we need to keep looking
           * for the implementation, so make the invocation bypass all further
           * visibility checks.
           *
           * This is pretty much always where a subclass marks a superclass
           * method as public. We don't move the method, we just put this
           * marker into the method table. */
          if(!vis->method()->nil_p()) {
            this->method = as<Executable>(vis->method());
            this->module = module;

            return true;
          }
        } else {
          this->method = as<Executable>(entry);
          this->module = module;

          return true;
        }
      }

      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return false;
    } while(1);

    // Shouldn't be here!
    rubinius::abort();
  }

  void InlineCache::run_wb(STATE, CompiledMethod* exec) {
    update_seen_classes();

    exec->write_barrier(state, module);
    exec->write_barrier(state, method);
    exec->write_barrier(state, klass_);
  }

  Object* InlineCache::empty_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Object* const recv = args.recv();
    cache->set_klass(recv->lookup_begin(state));

    if(!cache->fill_public(state, call_frame->self(), cache->name)) {
      assert(cache->fill_method_missing(state, cache->klass())
             && "no method_missing!");

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
    } else {
      cache->execute_backend_ = check_cache;
    }

    cache->run_wb(state, call_frame->cm);

    // If we overflow, disable the cache.
    if(cache->seen_classes_overflow() > 0) {
      cache->execute_backend_ = disabled_cache;
    }

    return cache->method->execute(state, call_frame, *cache, args);
  }

  Object* InlineCache::empty_cache_private(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Object* const recv = args.recv();
    cache->set_klass(recv->lookup_begin(state));

    if(!cache->fill_private(state, cache->name, cache->klass())) {
      assert(cache->fill_method_missing(state, cache->klass())
             && "no method_missing!");

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
    } else {
      cache->execute_backend_ = check_cache;
    }

    cache->run_wb(state, call_frame->cm);

    // If we overflow, disable the cache.
    if(cache->seen_classes_overflow() > 0) {
      cache->execute_backend_ = disabled_cache;
    }

    return cache->method->execute(state, call_frame, *cache, args);
  }

  Object* InlineCache::empty_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Object* const recv = args.recv();
    cache->set_klass(recv->lookup_begin(state));

    Module* const start = call_frame->module()->superclass();

    if(!cache->fill_private(state, cache->name, start)) {
      assert(cache->fill_method_missing(state,  start)
             && "no method_missing!");

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
    } else {
      cache->execute_backend_ = check_cache;
    }

    cache->run_wb(state, call_frame->cm);

    // If we overflow, disable the cache.
    if(cache->seen_classes_overflow() > 0) {
      cache->execute_backend_ = disabled_cache;
    }

    return cache->method->execute(state, call_frame, *cache, args);
  }

  Object* InlineCache::check_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    if(likely(cache->valid_p(state, args.recv()))) {
      *cache->hits_++;
      return cache->method->execute(state, call_frame, *cache, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    if(likely(cache->valid_p(state, args.recv()))) {
      *cache->hits_++;
      args.unshift(state, cache->name);
      return cache->method->execute(state, call_frame, *cache, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::disabled_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                      Arguments& args)
  {
    return cache->send(state, call_frame, args);
  }

  void InlineCacheRegistry::add_cache(Symbol* sym, InlineCache* cache) {
    // TODO make sure cache isn't already known?
    caches_[sym->index()].push_back(cache);
  }

  void InlineCacheRegistry::clear(Symbol* sym) {
    CacheVector& vec = caches_[sym->index()];

    for(CacheVector::iterator i = vec.begin();
        i != vec.end();
        i++) {
      (*i)->clear();
    }
  }
}
