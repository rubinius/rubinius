#include "arguments.hpp"
#include "call_frame.hpp"
#include "memory.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/mono_inline_cache.hpp"
#include "builtin/poly_inline_cache.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {

  void PolyInlineCache::bootstrap(STATE) {
    GO(poly_inline_cache).set(state->memory()->new_class<Class, PolyInlineCache>(
          state, G(call_site), G(rubinius), "PolyInlineCache"));

    GO(inline_cache_entry).set(state->memory()->new_class<Class, InlineCacheEntry>(
          state, G(call_site), G(rubinius), "InlineCacheEntry"));
  }

  InlineCacheEntry* InlineCacheEntry::create(STATE, ClassData data, Class* klass, Dispatch& dis, int hits) {
    InlineCacheEntry* cache =
      state->memory()->new_object_pinned<InlineCacheEntry>(state, G(inline_cache_entry));

    cache->receiver_data(data);
    cache->receiver_class(state, klass);
    cache->stored_module(state, dis.module);
    cache->method(state, dis.method);
    cache->method_missing(dis.method_missing);
    cache->hits(hits);

    state->vm()->metrics().machine.inline_cache_count++;

    return cache;
  }

  Integer* InlineCacheEntry::hits_prim(STATE) {
    return Integer::from(state, hits());
  }

  Symbol* InlineCacheEntry::method_missing_prim(STATE) {
    switch(method_missing()) {
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
      state->memory()->new_object_pinned<PolyInlineCache>(state, G(poly_inline_cache));
    cache->name(state, mono->name());
    cache->executable(state, mono->executable());
    cache->ip(mono->ip());
    cache->executor(check_cache);
    cache->fallback(mono->fallback());
    cache->updater(inline_cache_updater);
    cache->seen_classes_overflow(0);
    cache->clear();

    Dispatch dispatch(mono->name());
    dispatch.module = mono->stored_module();
    dispatch.method = mono->method();
    dispatch.method_missing = mono->method_missing();
    InlineCacheEntry* entry = InlineCacheEntry::create(
        state, mono->receiver_data(), mono->receiver_class(), dispatch, mono->hits());
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
    return Integer::from(state, seen_classes_overflow());
  }

  Object* PolyInlineCache::check_cache(STATE, CallSite* call_site,
                                   Arguments& args)
  {
    Class* const recv_class = args.recv()->direct_class(state);

    PolyInlineCache* cache = static_cast<PolyInlineCache*>(call_site);
    InlineCacheEntry* entry = cache->get_entry(recv_class);

    if(likely(entry)) {
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      entry->hit();
      state->vm()->metrics().machine.inline_cache_hits++;
      state->vm()->metrics().machine.methods_invoked++;

      return meth->execute(state, meth, mod, args);
    }

    state->vm()->metrics().machine.inline_cache_misses++;

    return cache->fallback(state, args);
  }

  Object* PolyInlineCache::check_cache_mm(STATE, CallSite* call_site,
                                      Arguments& args)
  {
    Class* const recv_class = args.recv()->direct_class(state);

    PolyInlineCache* cache = static_cast<PolyInlineCache*>(call_site);
    InlineCacheEntry* entry = cache->get_entry(recv_class);

    if(likely(entry)) {
      if(entry->method_missing() != eNone) {
        args.unshift(state, call_site->name());
        state->vm()->set_method_missing_reason(entry->method_missing());
      }
      Executable* meth = entry->method();
      Module* mod = entry->stored_module();
      entry->hit();
      state->vm()->metrics().machine.inline_cache_hits++;
      state->vm()->metrics().machine.methods_invoked++;

      return meth->execute(state, meth, mod, args);
    }

    state->vm()->metrics().machine.inline_cache_misses++;

    return cache->fallback(state, args);
  }

  void PolyInlineCache::inline_cache_updater(STATE, CallSite* call_site, Class* klass, Dispatch& dispatch) {
    PolyInlineCache* cache = reinterpret_cast<PolyInlineCache*>(call_site);
    InlineCacheEntry* entry = InlineCacheEntry::create(state,
        klass->class_data(), klass, dispatch, 1);
    cache->set_cache(state, entry);
  }

  void PolyInlineCache::print(STATE, std::ostream& stream) {
    stream << "name: " << name()->debug_str(state) << "\n"
           << "seen classes: " << classes_seen() << "\n"
           << "overflows: " << seen_classes_overflow() << "\n"
           << "classes:\n";

    for(int i = 0; i < cTrackedICHits; i++) {
      if(entries_[i]) {
        Module* mod = entries_[i]->receiver_class();
        if(mod) {
          if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
            if(Module* inner = try_as<Module>(sc->singleton())) {
              stream << "  SingletonClass:" << inner->debug_str(state);
            } else {
              stream << "  SingletonClass:" << sc->singleton()->class_object(state)->debug_str(state);
            }
          } else {
            stream << "  " << mod->debug_str(state);
          }

          stream << "\n";
        }
      }
    }
  }

  void PolyInlineCache::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);
    PolyInlineCache* cache = static_cast<PolyInlineCache*>(obj);

    for(int i = 0; i < cTrackedICHits; ++i) {
      InlineCacheEntry* ice = cache->entries_[i];
      if(ice) {
        if(InlineCacheEntry* updated = static_cast<InlineCacheEntry*>(mark.call(ice))) {
          cache->entries_[i] = updated;
          mark.just_set(cache, updated);
        }
      }
    }
  }

}
