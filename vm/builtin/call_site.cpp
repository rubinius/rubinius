#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "gc/gc.hpp"
#include "builtin/object.hpp"
#include "builtin/class.hpp"
#include "builtin/call_site.hpp"
#include "builtin/inline_cache.hpp"
#include "builtin/module.hpp"
#include "builtin/symbol.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/executable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/alias.hpp"
#include "builtin/call_unit.hpp"

#include "arguments.hpp"
#include "call_frame.hpp"
#include "ontology.hpp"

namespace rubinius {

  void CallSite::init(STATE) {
    GO(call_site).set(ontology::new_class(state, "CallSite", G(object), G(rubinius)));
    G(executable)->set_object_type(state, CallSiteType);
  }

  CallSite* CallSite::empty(STATE, Symbol* name, Executable* executable, int ip) {
    CallSite* cache =
      state->new_object_dirty<CallSite>(G(call_site));
    cache->name_ = name;
    cache->executor_ = empty_cache;
    cache->fallback_ = empty_cache;
    cache->executable(state, executable);
    cache->ip_ = ip;
    return cache;
  }

  Object* CallSite::empty_cache_custom(STATE, CallSite* call_site, CallFrame* call_frame,
                                          Arguments& args)
  {
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    Array*  ary = Array::create(state, args.total() + 2);
    ary->set(state, 0, recv);
    ary->set(state, 1, call_site->name_);

    for(size_t i = 0; i < args.total(); i++) {
      ary->set(state, i + 2, args.get_argument(i));
    }

    Object* ret = G(rubinius)->send(state, call_frame, state->symbol("bind_call"), ary);

    if(CallUnit* cu = try_as<CallUnit>(ret)) {
      InlineCache* cache = InlineCache::empty(state, call_site->name(),
                                              call_site->executable(),
                                              call_site->ip());
      cache->set_call_custom();
      InlineCacheEntry* ice = 0;

      ice = InlineCacheEntry::create(state, recv_class, cu->module(), cu->executable(), eNone);

      cache->write_barrier(state, ice);
      cache->set_cache(ice);
      cache->call_unit(state, cu);
      cache->executor_ = InlineCache::check_cache_custom;

      call_site->update_call_site(state, cache);

      return cu->execute(state, call_frame, cu, ice->method(), ice->stored_module(), args);
    } else {
      Exception::internal_error(state, call_frame, "bind_call must return CallUnit");
      return 0;
    }
  }

  Object* CallSite::empty_cache(STATE, CallSite* call_site, CallFrame* call_frame,
                                   Arguments& args)
  {
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    InlineCache* cache = try_as<InlineCache>(call_site);
    InlineCacheEntry* ice = NULL;

    if(!cache) {
      cache = InlineCache::empty(state, call_site->name(),
                                        call_site->executable(),
                                        call_site->ip());
    }

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
        cache->executor_ = InlineCache::check_cache_poly;
      } else {
        cache->executor_ = InlineCache::check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->executor_ = InlineCache::check_cache_poly;
      } else {
        cache->executor_ = InlineCache::check_cache;
      }
    }

    cache->write_barrier(state, ice);
    cache->set_cache(ice);
    call_site->update_call_site(state, cache);

    Executable* meth = ice->method();
    Module* mod = ice->stored_module();

    if(meth->custom_call_site_p()) {
      CallSiteInformation info(cache->executable(), cache->ip());
      state->set_call_site_information(&info);
      Object* res = meth->execute(state, call_frame, meth, mod, args);
      state->set_call_site_information(NULL);
      return res;
    } else {
      return meth->execute(state, call_frame, meth, mod, args);
    }
  }

  Object* CallSite::empty_cache_private(STATE, CallSite* call_site, CallFrame* call_frame,
                                   Arguments& args)
  {
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    InlineCacheEntry* ice = 0;
    InlineCache* cache = try_as<InlineCache>(call_site);

    if(!cache) {
      cache = InlineCache::empty(state, call_site->name(),
                                        call_site->executable(),
                                        call_site->ip());
      cache->set_is_private();
    }

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
        cache->executor_ = InlineCache::check_cache_poly;
      } else {
        cache->executor_ = InlineCache::check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->executor_ = InlineCache::check_cache_poly;
      } else {
        cache->executor_ = InlineCache::check_cache;
      }
    }

    cache->write_barrier(state, ice);
    cache->set_cache(ice);
    call_site->update_call_site(state, cache);

    Executable* meth = ice->method();
    Module* mod = ice->stored_module();

    if(meth->custom_call_site_p()) {
      CallSiteInformation info(cache->executable(), cache->ip());
      state->set_call_site_information(&info);
      Object* res = meth->execute(state, call_frame, meth, mod, args);
      state->set_call_site_information(NULL);
      return res;
    } else {
      return meth->execute(state, call_frame, meth, mod, args);
    }
  }

  Object* CallSite::empty_cache_vcall(STATE, CallSite* call_site, CallFrame* call_frame,
                                         Arguments& args)
  {
    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    InlineCacheEntry* ice = NULL;
    InlineCache* cache = try_as<InlineCache>(call_site);

    if(!cache) {
      cache = InlineCache::empty(state, call_site->name(),
                                        call_site->executable(),
                                        call_site->ip());
      cache->set_is_vcall();
    }

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
        cache->executor_ = InlineCache::check_cache_poly;
      } else {
        cache->executor_ = InlineCache::check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->executor_ = InlineCache::check_cache_poly;
      } else {
        cache->executor_ = InlineCache::check_cache;
      }
    }

    cache->write_barrier(state, ice);
    cache->set_cache(ice);
    call_site->update_call_site(state, cache);

    Executable* meth = ice->method();
    Module* mod = ice->stored_module();

    if(meth->custom_call_site_p()) {
      CallSiteInformation info(cache->executable(), cache->ip());
      state->set_call_site_information(&info);
      Object* res = meth->execute(state, call_frame, meth, mod, args);
      state->set_call_site_information(NULL);
      return res;
    } else {
      return meth->execute(state, call_frame, meth, mod, args);
    }
  }

  Object* CallSite::empty_cache_super(STATE, CallSite* call_site, CallFrame* call_frame,
                                   Arguments& args)
  {
    Symbol* original_name = call_frame->original_name();
    if(call_site->name_ != original_name) {
      call_site->name_ = original_name;
      args.set_name(call_site->name_);
    }

    Object* const recv = args.recv();
    Class* const recv_class = recv->lookup_begin(state);

    InlineCacheEntry* ice = 0;
    InlineCache* cache = try_as<InlineCache>(call_site);

    if(!cache) {
      cache = InlineCache::empty(state, call_site->name(),
                                        call_site->executable(),
                                        call_site->ip());
      cache->set_is_super();
    }

    Module* const start = call_frame->module()->superclass();

    bool parent_found = !start->nil_p();
    MethodMissingReason reason = eSuper;
    if(parent_found) {
      reason = cache->fill(state, call_frame->self(), recv_class,
                          cache->name_, start, G(sym_private), ice);

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
        cache->executor_ = InlineCache::check_cache_poly;
      } else {
        cache->executor_ = InlineCache::check_cache_mm;
      }
    } else {
      if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
        cache->executor_ = InlineCache::check_cache_poly;
      } else {
        cache->executor_ = InlineCache::check_cache;
      }
    }

    cache->write_barrier(state, ice);
    cache->set_cache(ice);
    call_site->update_call_site(state, cache);

    Executable* meth = ice->method();
    Module* mod = ice->stored_module();

    if(meth->custom_call_site_p()) {
      CallSiteInformation info(cache->executable(), cache->ip());
      state->set_call_site_information(&info);
      Object* res = meth->execute(state, call_frame, meth, mod, args);
      state->set_call_site_information(NULL);
      return res;
    } else {
      return meth->execute(state, call_frame, meth, mod, args);
    }
  }

  bool CallSite::update_and_validate(STATE, CallFrame* call_frame, Object* recv, Symbol* vis, int serial) {

    Class* const recv_class = recv->lookup_begin(state);

    InlineCache* cache = try_as<InlineCache>(this);

    if(!cache) {
      cache = InlineCache::empty(state, name_,
                                        executable_,
                                        ip_);
      cache->set_executor(executor_);
    }

    InlineCacheEntry* ice = cache->get_cache(recv_class);

    if(likely(ice)) return ice->method()->serial()->to_native() == serial;

    MethodMissingReason reason =
      cache->fill(state, call_frame->self(), recv_class, name_, recv_class, vis, ice);
    if(reason != eNone) return false;

    if(unlikely(cache->growth_cache_size(ice->receiver_class_id()) > 0)) {
      cache->executor_ = InlineCache::check_cache_poly;
    }

    cache->write_barrier(state, ice);
    cache->set_cache(ice);
    update_call_site(state, cache);

    return ice->method()->serial()->to_native() == serial;
  }

  void CallSite::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }

}

