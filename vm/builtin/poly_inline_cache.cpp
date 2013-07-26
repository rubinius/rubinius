#include "arguments.hpp"
#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/mono_inline_cache.hpp"
#include "builtin/poly_inline_cache.hpp"
#include "builtin/symbol.hpp"
#include "call_frame.hpp"
#include "ontology.hpp"

namespace rubinius {

  void PolyInlineCache::init(STATE) {
    GO(poly_inline_cache).set(
        ontology::new_class(state, "PolyInlineCache",
          G(call_site), G(rubinius)));
    G(poly_inline_cache)->set_object_type(state, PolyInlineCacheType);

    GO(inline_cache_entry).set(
        ontology::new_class(state, "InlineCacheEntry",
          G(call_site), G(rubinius)));
    G(inline_cache_entry)->set_object_type(state, InlineCacheEntryType);
  }

  InlineCacheEntry* InlineCacheEntry::create(STATE, ClassData data, Class* klass, Dispatch& dis, int hits) {
    InlineCacheEntry* cache = state->new_object_dirty<InlineCacheEntry>(G(inline_cache_entry));

    cache->receiver_ = data;
    cache->receiver_class(state, klass);
    cache->stored_module(state, dis.module);
    cache->method(state, dis.method);
    cache->method_missing_ = dis.method_missing;
    cache->hits_ = hits;

    return cache;
  }

  Integer* InlineCacheEntry::hits_prim(STATE) {
    return Integer::from(state, hits_);
  }

  Symbol* InlineCacheEntry::method_missing_prim(STATE) {
    switch(method_missing_) {
    case eNone:
      return state->symbol("none");
    case ePrivate:
      return G(sym_private);
    case eProtected:
      return G(sym_protected);
    case eSuper:
      return state->symbol("super");
    case eVCall:
      return state->symbol("vcall");
    case eNormal:
      return state->symbol("normal");
    }
    return nil<Symbol>();
  }

  PolyInlineCache* PolyInlineCache::create(STATE, MonoInlineCache* mono) {
    PolyInlineCache* cache =
      state->vm()->new_object_mature<PolyInlineCache>(G(poly_inline_cache));
    cache->name_     = mono->name();
    cache->executable(state, mono->executable());
    cache->ip_       = mono->ip();
    cache->executor_ = check_cache;
    cache->fallback_ = mono->fallback_;
    cache->updater_  = inline_cache_updater;
    cache->seen_classes_overflow_ = 0;
    cache->clear();

    Dispatch dis(mono->name());
    dis.module = mono->stored_module();
    dis.method = mono->method();
    dis.method_missing = mono->method_missing();
    InlineCacheEntry* entry = InlineCacheEntry::create(state, mono->receiver_data(),
                                                       mono->receiver_class(), dis, mono->hits());
    cache->set_cache(state, entry);
    return cache;
  }

  Tuple* PolyInlineCache::entries_prim(STATE) {
    Tuple* tup = Tuple::create(state, cTrackedICHits);
    for(int i = 0; i < cTrackedICHits; ++i) {
      InlineCacheEntry* ice = entries_[i];
      if(ice) tup->put(state, i, ice);
    }
    return tup;
  }

  Integer* PolyInlineCache::overflows(STATE) {
    return Integer::from(state, seen_classes_overflow_);
  }

  Object* PolyInlineCache::check_cache(STATE, CallSite* call_site, CallFrame* call_frame,
                                   Arguments& args)
  {
    Class* const recv_class = args.recv()->direct_class(state);

    PolyInlineCache* cache = static_cast<PolyInlineCache*>(call_site);
    InlineCacheEntry* entry = cache->get_entry(recv_class);

    if(likely(entry)) {
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      entry->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->fallback(state, call_frame, args);
  }

  Object* PolyInlineCache::check_cache_mm(STATE, CallSite* call_site, CallFrame* call_frame,
                                      Arguments& args)
  {
    Class* const recv_class = args.recv()->direct_class(state);

    PolyInlineCache* cache = static_cast<PolyInlineCache*>(call_site);
    InlineCacheEntry* entry = cache->get_entry(recv_class);

    if(likely(entry)) {
      if(entry->method_missing() != eNone) {
        args.unshift(state, call_site->name_);
        state->vm()->set_method_missing_reason(entry->method_missing());
      }
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      entry->hit();

      return meth->execute(state, call_frame, meth, mod, args);
    }

    return cache->fallback(state, call_frame, args);
  }

  void PolyInlineCache::inline_cache_updater(STATE, CallSite* call_site, Class* klass, Dispatch& dispatch) {
    PolyInlineCache* cache = reinterpret_cast<PolyInlineCache*>(call_site);
    InlineCacheEntry* entry = InlineCacheEntry::create(state, klass->data(), klass, dispatch, 1);
    cache->set_cache(state, entry);
  }

  void PolyInlineCache::print(STATE, std::ostream& stream) {
    stream << "name: " << name_->debug_str(state) << "\n"
           << "seen classes: " << classes_seen() << "\n"
           << "overflows: " << seen_classes_overflow_ << "\n"
           << "classes:\n";

    for(int i = 0; i < cTrackedICHits; i++) {
      if(entries_[i]) {
        Module* mod = entries_[i]->receiver_class();
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

  void PolyInlineCache::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
    PolyInlineCache* cache = static_cast<PolyInlineCache*>(obj);

    for(int i = 0; i < cTrackedICHits; ++i) {
      InlineCacheEntry* ice = cache->entries_[i];
      if(ice) {
        InlineCacheEntry* updated = static_cast<InlineCacheEntry*>(mark.call(ice));
        if(updated && updated != ice) {
          cache->entries_[i] = updated;
          mark.just_set(cache, updated);
        }
      }
    }
  }

}
