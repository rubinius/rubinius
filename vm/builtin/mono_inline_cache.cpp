#include "arguments.hpp"
#include "builtin/class.hpp"
#include "builtin/mono_inline_cache.hpp"
#include "builtin/poly_inline_cache.hpp"
#include "builtin/executable.hpp"
#include "call_frame.hpp"
#include "ontology.hpp"

namespace rubinius {

  void MonoInlineCache::init(STATE) {
    GO(mono_inline_cache).set(
        ontology::new_class(state, "MonoInlineCache",
          G(call_site), G(rubinius)));
    G(mono_inline_cache)->set_object_type(state, MonoInlineCacheType);
  }

  Integer* MonoInlineCache::hits_prim(STATE) {
    return Integer::from(state, hits_);
  }

  Symbol* MonoInlineCache::method_missing_prim(STATE) {
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

  MonoInlineCache* MonoInlineCache::create(STATE, CallSite* call_site, Class* klass, Dispatch& dis) {
    MonoInlineCache* cache = state->new_object_dirty<MonoInlineCache>(G(mono_inline_cache));
    cache->name_     = call_site->name();
    cache->executable(state, call_site->executable());
    cache->ip_       = call_site->ip();
    if(dis.method_missing == eNone) {
      cache->executor_ = check_cache;
    } else {
      cache->executor_ = check_cache_mm;
    }
    cache->fallback_ = call_site->fallback_;
    cache->updater_  = mono_cache_updater;

    cache->receiver_ = klass->data();
    cache->receiver_class(state, klass);
    cache->stored_module(state, dis.module);
    cache->method(state, dis.method);
    cache->method_missing_ = dis.method_missing;
    cache->hits_ = 1;

    return cache;
  }

  Object* MonoInlineCache::check_cache(STATE, CallSite* call_site, CallFrame* call_frame,
                                       Arguments& args) {

    MonoInlineCache* cache = reinterpret_cast<MonoInlineCache*>(call_site);
    Class* const recv_class = args.recv()->direct_class(state);

    register uint64_t recv_data = recv_class->data_raw();

    if(likely(cache->receiver_.raw == recv_data)) {
      cache->hits_++;
      return cache->method_->execute(state, call_frame, cache->method_, cache->stored_module_, args);
    }

    return call_site->fallback(state, call_frame, args);
  }

  Object* MonoInlineCache::check_cache_mm(STATE, CallSite* call_site, CallFrame* call_frame,
                                   Arguments& args) {
    MonoInlineCache* cache = reinterpret_cast<MonoInlineCache*>(call_site);

    Class* const recv_class = args.recv()->direct_class(state);

    register uint64_t recv_data = recv_class->data_raw();

    if(likely(cache->receiver_.raw == recv_data)) {
      cache->hits_++;
      state->vm()->set_method_missing_reason(cache->method_missing_);
      args.unshift(state, call_site->name_);
      return cache->method_->execute(state, call_frame, cache->method_, cache->stored_module_, args);
    }

    return call_site->fallback(state, call_frame, args);
  }

  void MonoInlineCache::mono_cache_updater(STATE, CallSite* call_site, Class* klass, Dispatch& dispatch) {
    MonoInlineCache* mono_cache = reinterpret_cast<MonoInlineCache*>(call_site);

    // If it's the same class, replace it since the old cache is
    // for an older version of the same class and we don't
    // want to go polymorphic in that case.
    if(klass == mono_cache->receiver_class_) {
      CallSite::empty_cache_updater(state, call_site, klass, dispatch);
    } else {
      PolyInlineCache* poly_cache = PolyInlineCache::create(state, mono_cache);
      InlineCacheEntry* entry = InlineCacheEntry::create(state, klass->data(),
                                                         klass, dispatch, 1);

      poly_cache->set_cache(state, entry);
      call_site->update_call_site(state, poly_cache);
    }
  }

  void MonoInlineCache::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }

}

