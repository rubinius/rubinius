#include "inline_cache.hpp"
#include "arguments.hpp"
#include "call_frame.hpp"
#include "global_cache.hpp"

#include "builtin/symbol.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/executable.hpp"
#include "builtin/methodtable.hpp"

namespace rubinius {

  bool InlineCache::fill_public(STATE, Object* self, Symbol* name) {
    MethodTableBucket* entry;

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
      entry = module->method_table()->find_entry(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(entry) {
        /* A 'false' method means to terminate method lookup.
         * (eg. undef_method) */
        if(entry->undef_p(state)) return false;

        if(!skip_vis_check) {
          /* The method is private, but this wasn't a private send. */
          if(entry->private_p(state)) {
            return false;
          } else if(entry->protected_p(state)) {
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
        if(entry->method()->nil_p()) {
          skip_vis_check = true;
        } else {
          this->method = entry->method();
          this->module = module;

          state->global_cache->retain(state, klass_, name, this->module,
                                      this->method, false,
                                      !entry->public_p(state));
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
    MethodTableBucket* entry;

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
          this->method = entry->method();
          this->module = module;
          state->global_cache->retain(state, start, name, this->module,
              this->method, false,
              !entry->public_p(state));

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
    MethodTableBucket* entry;

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
          this->method = entry->method();
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

    cache->update_seen_classes();
    cache->run_wb(state, call_frame->cm);

    /*
    // If we overflow, disable the cache.
    if(cache->seen_classes_overflow() > 0) {
      cache->execute_backend_ = disabled_cache;
    }
    */

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

    cache->update_seen_classes();
    cache->run_wb(state, call_frame->cm);

    /*
    // If we overflow, disable the cache.
    if(cache->seen_classes_overflow() > 0) {
      cache->execute_backend_ = disabled_cache_private;
    }
    */

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

    cache->update_seen_classes();
    cache->run_wb(state, call_frame->cm);

    /*
    // If we overflow, disable the cache.
    if(cache->seen_classes_overflow() > 0) {
      cache->execute_backend_ = disabled_cache_super;
    }
    */

    return cache->method->execute(state, call_frame, *cache, args);
  }

  Object* InlineCache::check_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    if(likely(cache->valid_p(state, args.recv()))) {
      cache->inc_hits();
      return cache->method->execute(state, call_frame, *cache, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    if(likely(cache->valid_p(state, args.recv()))) {
      cache->inc_hits();
      args.unshift(state, cache->name);
      return cache->method->execute(state, call_frame, *cache, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::disabled_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                      Arguments& args)
  {
    if(!cache->fill_public(state, call_frame->self(), cache->name)) {
      assert(cache->fill_method_missing(state, cache->klass())
             && "no method_missing!");

      args.unshift(state, cache->name);
    }

    cache->run_wb(state, call_frame->cm);

    return cache->method->execute(state, call_frame, *cache, args);
  }

  Object* InlineCache::disabled_cache_private(STATE, InlineCache* cache, CallFrame* call_frame,
                                      Arguments& args)
  {
    if(!cache->fill_private(state, cache->name, cache->klass())) {
      assert(cache->fill_method_missing(state, cache->klass())
             && "no method_missing!");

      args.unshift(state, cache->name);
    }

    cache->run_wb(state, call_frame->cm);

    return cache->method->execute(state, call_frame, *cache, args);
  }

  Object* InlineCache::disabled_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Module* const start = call_frame->module()->superclass();

    if(!cache->fill_private(state, cache->name, start)) {
      assert(cache->fill_method_missing(state,  start)
             && "no method_missing!");

      args.unshift(state, cache->name);
    }

    return cache->method->execute(state, call_frame, *cache, args);
  }

  void InlineCache::update_seen_classes() {
    for(int i = 0; i < cTrackedICHits; i++) {
      Module* mod = seen_classes_[i].klass();
      if(mod == klass_) {
        hits_ = seen_classes_[i].hits_address();
        return;

      // Use any slot that has 0 hits. This means we kick out
      // entries that are stored here, but never used.
      } else if(seen_classes_[i].hits() == 0) {
        // An empty space, record it.
        hits_ = seen_classes_[i].assign(klass_);
        return;
      }
    }

    // Hmmm, what do we do when this is full? Just ignore them?
    // For now, just keep track of how many times we overflow.
    seen_classes_overflow_++;
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
    stream << "name: " << name->c_str(state) << "\n"
           << "seen classes: " << classes_seen() << "\n"
           << "total hits: " << total_hits() << "\n"
           << "overflows: " << seen_classes_overflow_ << "\n"
           << "classes:\n";

    for(int i = 0; i < cTrackedICHits; i++) {
      Module* mod = seen_classes_[i].klass();
      if(mod) {
        if(MetaClass* mc = try_as<MetaClass>(mod)) {
          if(Module* inner = try_as<Module>(mc->attached_instance())) {
            stream << "  MetaClass:" << inner->name()->c_str(state);
          } else {
            stream << "  MetaClass:" << mc->attached_instance()->class_object(state)->name()->c_str(state);
          }
        } else {
          stream << "  " << mod->name()->c_str(state);
        }

        stream << " " << seen_classes_[i].hits() << "\n";
      }
    }
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

  static inline double ratio(int part, int total) {
    return 100 * ((double)part / (double)total);
  }

  void InlineCacheRegistry::print_stats(STATE) {
    int total = 0;
    std::vector<int> sizes(cTrackedICHits + 1);

    int overflow = 0;

    for(CacheHash::iterator hi = caches_.begin();
        hi != caches_.end();
        hi++) {
      for(CacheVector::iterator vi = hi->second.begin();
          vi != hi->second.end();
          vi++) {
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

    // Stats that take the number of hits into account
    std::vector<int> hits(cTrackedICHits + 1);
    int overflow_hits = 0;
    int total_hits = 0;

    for(CacheHash::iterator hi = caches_.begin();
        hi != caches_.end();
        hi++) {
      for(CacheVector::iterator vi = hi->second.begin();
          vi != hi->second.end();
          vi++) {
        InlineCache* ic = *vi;
        int seen = ic->classes_seen();
        if(ic->seen_classes_overflow() > 0) {
          int these_hits = (ic->total_hits() + ic->seen_classes_overflow());
          overflow_hits += these_hits;
          total_hits += these_hits;
        } else if(seen > 0) {
          hits[seen] += ic->total_hits();
          total_hits += ic->total_hits();
        }
      }
    }

    std::cerr << "Hits per classes tracked: (" << total_hits << ")\n";
    for(int i = 1; i < cTrackedICHits + 1; i++) {
      std::cerr << " " << i << ": " << hits[i] << " "
                << ratio(hits[i], total_hits) << "%\n";
    }

    std::cerr << cTrackedICHits << "+: " << overflow_hits << " "
              << ratio(overflow_hits, total_hits) << "%\n";

    // print out the mega-morphic ones
    std::cerr << "\nMegamorphic call sites:\n";

    for(CacheHash::iterator hi = caches_.begin();
        hi != caches_.end();
        hi++) {
      for(CacheVector::iterator vi = hi->second.begin();
          vi != hi->second.end();
          vi++) {
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
