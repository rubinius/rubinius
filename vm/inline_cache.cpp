#include "inline_cache.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"

#include "builtin/symbol.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/executable.hpp"
#include "builtin/methodvisibility.hpp"

namespace rubinius {

  bool InlineCache::fill_public(STATE, Object* self, Symbol* name) {
    Object* entry;
    MethodVisibility* vis;

    Module* module = klass_;

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
          return true;
        }
      }

      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) return false;
    } while(1);

    return true;
  }

  bool InlineCache::fill_private(STATE, Symbol* name, Module* module) {
    Object* entry;
    MethodVisibility* vis;

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

    return true;
  }

  void InlineCache::run_wb(STATE, CompiledMethod* exec) {
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
      assert(cache->fill_private(state, G(sym_method_missing), cache->klass())
             && "no method_missing!");

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
    } else {
      cache->execute_backend_ = check_cache;
    }

    cache->run_wb(state, call_frame->cm);

    return cache->method->execute(state, call_frame, *cache, args);
  }

  Object* InlineCache::empty_cache_private(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Object* const recv = args.recv();
    cache->set_klass(recv->lookup_begin(state));

    if(!cache->fill_private(state, cache->name, cache->klass())) {
      assert(cache->fill_private(state, G(sym_method_missing), cache->klass())
             && "no method_missing!");

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
    } else {
      cache->execute_backend_ = check_cache;
    }

    cache->run_wb(state, call_frame->cm);

    return cache->method->execute(state, call_frame, *cache, args);
  }

  Object* InlineCache::empty_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Object* const recv = args.recv();
    cache->set_klass(recv->lookup_begin(state));

    Module* const start = call_frame->module()->superclass();

    if(!cache->fill_private(state, cache->name, start)) {
      assert(cache->fill_private(state, G(sym_method_missing), start)
             && "no method_missing!");

      args.unshift(state, cache->name);
      cache->execute_backend_ = check_cache_mm;
    } else {
      cache->execute_backend_ = check_cache;
    }

    cache->run_wb(state, call_frame->cm);

    return cache->method->execute(state, call_frame, *cache, args);
  }

  Object* InlineCache::check_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    if(likely(cache->valid_p(state, args.recv()))) {
      return cache->method->execute(state, call_frame, *cache, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    if(likely(cache->valid_p(state, args.recv()))) {
      args.unshift(state, cache->name);
      return cache->method->execute(state, call_frame, *cache, args);
    }

    return cache->initialize(state, call_frame, args);
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
