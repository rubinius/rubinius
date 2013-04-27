#include "detection.hpp"

#include "builtin/mono_inline_cache.hpp"
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

  void MonoInlineCache::init(STATE) {
    GO(mono_inline_cache).set(
        ontology::new_class(state, "MonoInlineCache",
          G(call_site), G(rubinius)));
    G(mono_inline_cache)->set_object_type(state, MonoInlineCacheType);
  }

  MonoInlineCache* MonoInlineCache::empty(STATE, Symbol* name, Executable* executable, int ip) {
    MonoInlineCache* cache =
      state->new_object_dirty<MonoInlineCache>(G(mono_inline_cache));
    cache->name_ = name;
    cache->executor_ = CallSite::empty_cache;
    cache->fallback_ = CallSite::empty_cache;
    cache->updater_  = mono_cache_updater;
    cache->executable(state, executable);
    cache->ip_ = ip;
    return cache;
  }

  void MonoInlineCache::update(STATE, Class* klass, Module* mod, Executable* exec, MethodMissingReason reason) {
    receiver_class(state, klass);
    stored_module(state, mod);
    method(state, exec);
    receiver_ = klass->data();
    method_missing_ = reason;
  }

  Object* MonoInlineCache::check_cache(STATE, CallSite* call_site, CallFrame* call_frame,
                                       Arguments& args) {

    MonoInlineCache* cache = reinterpret_cast<MonoInlineCache*>(call_site);
    Class* const recv_class = args.recv()->lookup_begin(state);

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

    Class* const recv_class = args.recv()->lookup_begin(state);

    register uint64_t recv_data = recv_class->data_raw();

    if(likely(cache->receiver_.raw == recv_data)) {
      cache->hits_++;
      state->vm()->set_method_missing_reason(cache->method_missing_);
      args.unshift(state, call_site->name_);
      return cache->method_->execute(state, call_frame, cache->method_, cache->stored_module_, args);
    }

    return call_site->fallback(state, call_frame, args);
  }

  void MonoInlineCache::mono_cache_updater(STATE, CallSite* call_site, Class* klass, FallbackExecutor fallback, Dispatch& dispatch) {
    MonoInlineCache* existing_cache = reinterpret_cast<MonoInlineCache*>(call_site);

    // If it's the same class, replace it since the old cache is
    // for an older version of the same class and we don't
    // want to go polymorphic in that case.
    if(klass == existing_cache->receiver_class_) {
      CallSite::empty_cache_updater(state, call_site, klass, fallback, dispatch);
    } else {

      InlineCache* new_cache = InlineCache::empty(state, call_site->name(),
                                                  call_site->executable(),
                                                  call_site->ip());

      InlineCacheEntry* new_entry = InlineCacheEntry::create(state, klass->data(),
                                                                    klass,
                                                                    dispatch.module,
                                                                    dispatch.method,
                                                                    dispatch.method_missing);

      InlineCacheEntry* existing_entry = InlineCacheEntry::create(state, existing_cache->receiver_,
                                                                         existing_cache->receiver_class_,
                                                                         existing_cache->stored_module_,
                                                                         existing_cache->method_,
                                                                         existing_cache->method_missing_);

      new_cache->write_barrier(state, existing_entry);
      new_cache->set_cache(existing_entry);
      new_cache->write_barrier(state, new_entry);
      new_cache->set_cache(new_entry);
      if(new_entry->method_missing() == eNone && existing_entry->method_missing() == eNone) {
        new_cache->executor_ = InlineCache::check_cache_poly;
      } else {
        new_cache->executor_ = InlineCache::check_cache_poly_mm;
      }
      new_cache->fallback_ = fallback;

      existing_cache->update_call_site(state, new_cache);
    }
  }

  void MonoInlineCache::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }

}

