#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "gc/gc.hpp"
#include "builtin/object.hpp"
#include "builtin/class.hpp"
#include "builtin/call_site.hpp"
#include "builtin/mono_inline_cache.hpp"
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
    G(call_site)->set_object_type(state, CallSiteType);
  }

  CallSite* CallSite::empty(STATE, Symbol* name, Executable* executable, int ip) {
    CallSite* cache =
      state->new_object_dirty<CallSite>(G(call_site));
    cache->name_ = name;
    cache->executor_ = empty_cache;
    cache->fallback_ = empty_cache;
    cache->updater_  = empty_cache_updater;
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

      ice = InlineCacheEntry::create(state, recv_class->data(), recv_class, cu->module(), cu->executable(), eNone);

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

    LookupData lookup(call_frame->self(), recv_class, G(sym_public));
    Dispatch dis(call_site->name());

    if(!dis.resolve(state, call_site->name(), lookup)) {

      LookupData missing_lookup(call_frame->self(), recv_class, G(sym_private));
      Dispatch missing_dis(G(sym_method_missing));
      missing_dis.resolve(state, G(sym_method_missing), missing_lookup);

      if(missing_dis.method_missing != eNone) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, call_site->name());
      dis.method = missing_dis.method;
      dis.module = missing_dis.module;
      state->vm()->set_method_missing_reason(dis.method_missing);
    }

    call_site->update(state, recv_class, empty_cache, dis);

    Executable* meth = dis.method;
    Module* mod = dis.module;

    if(meth->custom_call_site_p()) {
      CallSiteInformation info(call_site->executable(), call_site->ip());
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

    LookupData lookup(call_frame->self(), recv_class, G(sym_private));
    Dispatch dis(call_site->name());

    if(!dis.resolve(state, call_site->name(), lookup)) {

      LookupData missing_lookup(call_frame->self(), recv_class, G(sym_private));
      Dispatch missing_dis(G(sym_method_missing));
      missing_dis.resolve(state, G(sym_method_missing), missing_lookup);

      if(missing_dis.method_missing != eNone) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, call_site->name());
      dis.method = missing_dis.method;
      dis.module = missing_dis.module;
      state->vm()->set_method_missing_reason(dis.method_missing);
    }

    call_site->update(state, recv_class, empty_cache_private, dis);

    Executable* meth = dis.method;
    Module* mod = dis.module;

    if(meth->custom_call_site_p()) {
      CallSiteInformation info(call_site->executable(), call_site->ip());
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

    LookupData lookup(call_frame->self(), recv_class, G(sym_private));
    Dispatch dis(call_site->name());

    if(!dis.resolve(state, call_site->name(), lookup)) {

      LookupData missing_lookup(call_frame->self(), recv_class, G(sym_private));
      Dispatch missing_dis(G(sym_method_missing));
      missing_dis.resolve(state, G(sym_method_missing), missing_lookup);

      if(missing_dis.method_missing != eNone) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, call_site->name());
      dis.method = missing_dis.method;
      dis.module = missing_dis.module;
      dis.method_missing = eVCall;
      state->vm()->set_method_missing_reason(dis.method_missing);
    }

    call_site->update(state, recv_class, empty_cache_vcall, dis);

    Executable* meth = dis.method;
    Module* mod = dis.module;

    if(meth->custom_call_site_p()) {
      CallSiteInformation info(call_site->executable(), call_site->ip());
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
    Module* const start = call_frame->module()->superclass();


    LookupData lookup(call_frame->self(), start, G(sym_private));
    Dispatch dis(call_site->name());

    if(start->nil_p() || !dis.resolve(state, call_site->name(), lookup)) {

      LookupData missing_lookup(call_frame->self(), recv_class, G(sym_private));
      Dispatch missing_dis(G(sym_method_missing));
      missing_dis.resolve(state, G(sym_method_missing), missing_lookup);

      if(missing_dis.method_missing != eNone) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      args.unshift(state, call_site->name());
      dis.method = missing_dis.method;
      dis.module = missing_dis.module;
      dis.method_missing = eSuper;
      state->vm()->set_method_missing_reason(dis.method_missing);
    }

    call_site->update(state, recv_class, empty_cache_super, dis);

    Executable* meth = dis.method;
    Module* mod = dis.module;

    if(meth->custom_call_site_p()) {
      CallSiteInformation info(call_site->executable(), call_site->ip());
      state->set_call_site_information(&info);
      Object* res = meth->execute(state, call_frame, meth, mod, args);
      state->set_call_site_information(NULL);
      return res;
    } else {
      return meth->execute(state, call_frame, meth, mod, args);
    }
  }

  void CallSite::empty_cache_updater(STATE, CallSite* call_site, Class* klass, FallbackExecutor fallback, Dispatch& dispatch) {
    MonoInlineCache* cache = MonoInlineCache::empty(state, call_site->name(),
                                                   call_site->executable(),
                                                   call_site->ip());
    cache->update(state, klass, dispatch.module, dispatch.method, dispatch.method_missing);
    if(dispatch.method_missing == eNone) {
      cache->executor_ = MonoInlineCache::check_cache;
    } else {
      cache->executor_ = MonoInlineCache::check_cache_mm;
    }
    cache->fallback_ = fallback;
    call_site->update_call_site(state, cache);
  }

  bool CallSite::update_and_validate(STATE, CallFrame* call_frame, Object* recv, Symbol* vis, int serial) {

    Class* const recv_class = recv->lookup_begin(state);

    if(MonoInlineCache* mono = try_as<MonoInlineCache>(this)) {
      if(recv_class->data_raw() == mono->receiver_data_raw()) {
        return mono->method()->serial()->to_native() == serial;
      }
    }

    if(InlineCache* cache = try_as<InlineCache>(this)) {
      InlineCacheEntry* ice = cache->get_cache(recv_class);
      if(likely(ice)) return ice->method()->serial()->to_native() == serial;
    }

    LookupData lookup(call_frame->self(), recv_class, G(sym_public));
    Dispatch dis(name_);

    if(dis.resolve(state, name_, lookup)) {
      update(state, recv_class, fallback_, dis);
      return dis.method->serial()->to_native() == serial;
    }
    return false;
  }

  void CallSite::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }

}

