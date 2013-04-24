#include "detection.hpp"

#ifdef OS_X_10_5
#ifndef RBX_HAVE_TR1_HASH
#include "missing/leopard_hashtable.hpp"
#endif
#endif

#include "builtin/inline_cache.hpp"
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
#include "ontology.hpp"

namespace rubinius {

  void InlineCache::init(STATE) {
    GO(inline_cache).set(
        ontology::new_class(state, "InlineCache",
          G(object), G(rubinius)));

  }

  InlineCache* InlineCache::empty(STATE, Symbol* name) {
    InlineCache* cache =
      state->vm()->new_object_mature<InlineCache>(G(inline_cache));
    cache->name_ = name;
    cache->call_unit_ = nil<CallUnit>();
    cache->initial_backend_ = empty_cache;
    cache->execute_backend_ = empty_cache;
    cache->seen_classes_overflow_ = 0;
    cache->clear();
    return cache;
  }

  InlineCacheEntry* InlineCacheEntry::create(STATE, Class* klass, Module* mod,
                                             Executable* exec, MethodMissingReason method_missing,
                                             bool super) {
    InlineCacheEntry* cache =
      state->new_object_dirty<InlineCacheEntry>(G(object));

    cache->stored_module(state, mod);
    cache->receiver_class(state, klass);
    cache->method(state, exec);
    cache->receiver_.f.class_id = klass->class_id();
    cache->receiver_.f.serial_id = klass->serial_id();
    cache->method_missing_ = method_missing;
    cache->super_ = super;

    return cache;
  }

  MethodMissingReason InlineCache::fill(STATE, Object* self, Class* klass,
                                        Symbol* name, Module* start, Symbol* vis,
                                        InlineCacheEntry*& ice,
                                        bool super)
  {
    LookupData lookup(self, start, vis);
    Dispatch dis(name);

    if(dis.resolve(state, name, lookup)) {
      ice = InlineCacheEntry::create(state, klass, dis.module, dis.method, dis.method_missing, super);
    }
    return dis.method_missing;
  }

  bool InlineCache::fill_method_missing(STATE, Object* self, Class* klass,
                                        MethodMissingReason reason,
                                        InlineCacheEntry*& ice)
  {

    LookupData lookup(self, klass, G(sym_private));
    Dispatch dis(G(sym_method_missing));

    if(dis.resolve(state, G(sym_method_missing), lookup)) {
      ice = InlineCacheEntry::create(state, klass, dis.module, dis.method, reason, false);
    }
    return dis.method_missing == eNone;
  }

  InlineCacheEntry* InlineCache::update_and_validate(STATE, CallFrame* call_frame, Object* recv, Symbol* vis) {

    Class* const recv_class = recv->lookup_begin(state);
    InlineCacheEntry* ice = get_cache(recv_class);

    if(likely(ice)) return ice;

    MethodMissingReason reason =
      fill(state, call_frame->self(), recv_class, name_, recv_class, vis, ice);
    if(reason != eNone) return 0;

    if(unlikely(growth_cache_size(ice->receiver_class_id()) > 0)) {
      execute_backend_ = check_cache_poly;
    }

    write_barrier(state, ice);
    set_cache(ice);

    return ice;
  }

  Object* InlineCache::empty_cache_custom(STATE, InlineCache* cache, CallFrame* call_frame,
                                          Arguments& args)
  {
    args.set_name(cache->name_);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    Array*  ary = Array::create(state, args.total() + 2);
    ary->set(state, 0, recv);
    ary->set(state, 1, cache->name_);

    for(size_t i = 0; i < args.total(); i++) {
      ary->set(state, i + 2, args.get_argument(i));
    }

    Object* ret = G(rubinius)->send(state, call_frame, state->symbol("bind_call"), ary);

    if(!ret) return 0;

    if(CallUnit* cu = try_as<CallUnit>(ret)) {
      InlineCacheEntry* ice = 0;

      ice = InlineCacheEntry::create(state, recv_class, cu->module(), cu->executable(), eNone, false);

      cache->write_barrier(state, ice);
      cache->set_cache(ice);
      cache->call_unit(state, cu);
      cache->execute_backend_ = check_cache_custom;

      return cu->execute(state, call_frame, cu, ice->method(), ice->stored_module(), args);
    } else {
      Exception::internal_error(state, call_frame, "bind_call must return CallUnit");
      return 0;
    }
  }

  Object* InlineCache::check_cache_custom(STATE, InlineCache* cache,
      CallFrame* call_frame, Arguments& args)
  {
    InlineCacheEntry* ice = cache->get_single_cache();

    args.set_name(cache->name_);
    return cache->call_unit_->execute(state, call_frame, cache->call_unit_,
                                      ice->method(), ice->stored_module(), args);
  }



  Object* InlineCache::empty_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    args.set_name(cache->name_);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    InlineCacheEntry* ice = NULL;

    MethodMissingReason reason =
      cache->fill(state, call_frame->self(), recv_class,
                         cache->name_, recv_class, G(sym_public), ice);

    if(reason != eNone) {
      state->vm()->set_method_missing_reason(reason);

      if(!cache->fill_method_missing(state, call_frame->self(), recv_class, reason, ice)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name_);
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache;
      }
    }

    cache->write_barrier(state, ice);
    cache->set_cache(ice);

    Executable* meth = ice->method();
    Module* mod = ice->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::empty_cache_private(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    args.set_name(cache->name_);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    InlineCacheEntry* ice = 0;

    MethodMissingReason reason =
      cache->fill(state, call_frame->self(), recv_class,
                         cache->name_, recv_class, G(sym_private), ice);

    if(reason != eNone) {
      state->vm()->set_method_missing_reason(eNormal);

      if(!cache->fill_method_missing(state, call_frame->self(), recv_class, reason, ice)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name_);
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache;
      }
    }

    cache->write_barrier(state, ice);
    cache->set_cache(ice);

    Executable* meth = ice->method();
    Module* mod = ice->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::empty_cache_vcall(STATE, InlineCache* cache, CallFrame* call_frame,
                                         Arguments& args)
  {
    args.set_name(cache->name_);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    InlineCacheEntry* ice = NULL;

    MethodMissingReason reason =
      cache->fill(state, call_frame->self(), recv_class,
                         cache->name_, recv_class, G(sym_private), ice);

    if(reason != eNone) {
      state->vm()->set_method_missing_reason(eVCall);

      if(!cache->fill_method_missing(state, call_frame->self(), recv_class, reason, ice)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name_);
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache;
      }
    }

    cache->write_barrier(state, ice);
    cache->set_cache(ice);

    Executable* meth = ice->method();
    Module* mod = ice->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::empty_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Symbol* original_name = call_frame->original_name();
    if(cache->name_ != original_name) {
      cache->name_ = original_name;
    }

    args.set_name(cache->name_);
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    InlineCacheEntry* ice = 0;

    Module* const start = call_frame->module()->superclass();

    bool parent_found = !start->nil_p();
    MethodMissingReason reason = eSuper;
    if(parent_found) {
      reason = cache->fill(state, call_frame->self(), recv_class,
                          cache->name_, start, G(sym_private), ice, true);

    }

    if(!parent_found || reason != eNone) {
      state->vm()->set_method_missing_reason(eSuper);

      // Don't use start when looking up method_missing!
      // Always completely redispatch for method_missing.
      // github#157
      if(!cache->fill_method_missing(state, call_frame->self(), recv_class, reason, ice)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, cache->name_);

      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->execute_backend_ = check_cache_poly;
      } else {
        cache->execute_backend_ = check_cache;
      }
    }

    cache->write_barrier(state, ice);
    cache->set_cache(ice);

    Executable* meth = ice->method();
    Module* mod = ice->stored_module();

    return meth->execute(state, call_frame, meth, mod, args);
  }

  Object* InlineCache::check_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Class* const recv_class = args.recv()->lookup_begin(state);

    InlineCacheEntry* entry;
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
    Class* const recv_class = args.recv()->lookup_begin(state);

    InlineCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(recv_class, entry);

    if(likely(ic)) {
      args.unshift(state, cache->name_);

      state->vm()->set_method_missing_reason(entry->method_missing());
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      ic->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->initialize(state, call_frame, args);
  }

  Object* InlineCache::check_cache_poly(STATE, InlineCache* cache, CallFrame* call_frame,
                                   Arguments& args)
  {
    Class* const recv_class = args.recv()->lookup_begin(state);

    InlineCacheEntry* entry;
    InlineCacheHit* ic = cache->get_inline_cache(recv_class, entry);

    if(likely(ic)) {
      if(entry->method_missing() != eNone) {
        args.unshift(state, cache->name_);
      }
      if(entry->super()) {
        Symbol* current_name = call_frame->original_name();
        if(current_name != cache->name_) {
          cache->name_ = current_name;
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

  void InlineCache::print(STATE, std::ostream& stream) {
    stream << "name: " << name_->debug_str(state) << "\n"
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

  void InlineCache::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
    InlineCache* cache = static_cast<InlineCache*>(obj);

    for(int j = 0; j < cTrackedICHits; ++j) {
      InlineCacheEntry* ice = cache->cache_[j].entry();
      if(ice) {
        InlineCacheEntry* updated = static_cast<InlineCacheEntry*>(mark.call(ice));
        if(updated) {
          cache->cache_[j].update(updated);
          mark.just_set(cache, updated);
        }
      }
    }

    if(cache->call_unit_) {
      CallUnit* updated = static_cast<CallUnit*>(mark.call(cache->call_unit_));
      if(updated) {
        cache->call_unit_ = static_cast<CallUnit*>(updated);
        mark.just_set(cache, updated);
      }
    }
  }

}
