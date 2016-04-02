#include "arguments.hpp"
#include "call_frame.hpp"
#include "global_cache.hpp"
#include "lookup_data.hpp"
#include "object_utils.hpp"
#include "memory.hpp"

#include "builtin/class.hpp"
#include "builtin/call_site.hpp"
#include "builtin/exception.hpp"
#include "builtin/executable.hpp"
#include "builtin/mono_inline_cache.hpp"
#include "builtin/object.hpp"
#include "builtin/poly_inline_cache.hpp"

#include <sstream>

namespace rubinius {

  void CallSite::bootstrap(STATE) {
    GO(call_site).set(state->memory()->new_class<Class, CallSite>(
          state, G(rubinius), "CallSite"));
  }

  CallSite* CallSite::empty(STATE, Symbol* name, Executable* executable, int ip) {
    CallSite* cache = state->memory()->new_object<CallSite>(state, G(call_site));

    cache->name(name);
    cache->executable(state, executable);
    cache->ip(ip);

    return cache;
  }

  Integer* CallSite::ip_prim(STATE) {
    return Integer::from(state, ip());
  }

  Object* CallSite::empty_cache(STATE, CallSite* call_site, Arguments& args) {
    Object* const self = state->vm()->call_frame()->self();
    Object* const recv = args.recv();
    Class*  const recv_class  = recv->direct_class(state);

    LookupData lookup(self, recv->lookup_begin(state), G(sym_public));
    Dispatch dispatch(call_site->name());

    if(!dispatch.resolve(state, call_site->name(), lookup)) {
      if(!lookup_method_missing(state, args,
          dispatch, self, recv->lookup_begin(state))) {
        return NULL;
      }
    }

    state->vm()->metrics().machine.methods_invoked++;

    call_site->update(state, recv_class, dispatch);

    Executable* meth = dispatch.method;
    Module* mod = dispatch.module;

    return meth->execute(state, meth, mod, args);
  }

  Object* CallSite::empty_cache_private(STATE, CallSite* call_site,
                                   Arguments& args)
  {
    Object* const self = state->vm()->call_frame()->self();
    Object* const recv = args.recv();
    Class* const recv_class = recv->direct_class(state);

    LookupData lookup(self, recv->lookup_begin(state), G(sym_private));
    Dispatch dispatch(call_site->name());

    if(!dispatch.resolve(state, dispatch.name, lookup)) {
      if(!lookup_method_missing(state, args,
          dispatch, self, recv->lookup_begin(state))) {
        return NULL;
      }
    }

    state->vm()->metrics().machine.methods_invoked++;

    call_site->update(state, recv_class, dispatch);

    Executable* meth = dispatch.method;
    Module* mod = dispatch.module;

    return meth->execute(state, meth, mod, args);
  }

  Object* CallSite::empty_cache_vcall(STATE, CallSite* call_site, Arguments& args) {
    Object* const self = state->vm()->call_frame()->self();
    Object* const recv = args.recv();
    Class* const recv_class = recv->direct_class(state);

    LookupData lookup(self, recv->lookup_begin(state), G(sym_private));
    Dispatch dispatch(call_site->name());

    if(!dispatch.resolve(state, call_site->name(), lookup)) {
      dispatch.method_missing = eVCall;
      if(!lookup_method_missing(state, args,
          dispatch, self, recv->lookup_begin(state))) {
        return NULL;
      }
    }

    state->vm()->metrics().machine.methods_invoked++;

    call_site->update(state, recv_class, dispatch);

    Executable* meth = dispatch.method;
    Module* mod = dispatch.module;

    return meth->execute(state, meth, mod, args);
  }

  Object* CallSite::empty_cache_super(STATE, CallSite* call_site,
                                   Arguments& args)
  {
    CallFrame* call_frame = state->vm()->call_frame();

    Symbol* original_name = call_frame->original_name();
    if(call_site->name() != original_name) {
      call_site->name(original_name);
      args.set_name(call_site->name());
    }

    Object* const recv = args.recv();
    Class* const recv_class = recv->direct_class(state);
    Module* const start = call_frame->module()->superclass();

    LookupData lookup(call_frame->self(), start, G(sym_private));
    Dispatch dispatch(call_site->name());

    if(start->nil_p() || !dispatch.resolve(state, call_site->name(), lookup)) {

      LookupData missing_lookup(call_frame->self(),
          recv->lookup_begin(state), G(sym_private));
      Dispatch missing_dispatch(G(sym_method_missing));

      if(!missing_dispatch.resolve(state, G(sym_method_missing), missing_lookup)) {
        std::ostringstream msg;
        msg << "no method_missing for ";
        msg << recv_class->to_string(state);
        msg << "#" << call_site->name()->to_string(state);

        Exception::internal_error(state, msg.str().c_str());
        return 0;
      }

      args.unshift(state, call_site->name());
      dispatch.method = missing_dispatch.method;
      dispatch.module = missing_dispatch.module;
      dispatch.method_missing = eSuper;
      state->vm()->set_method_missing_reason(dispatch.method_missing);
      state->vm()->global_cache()->add_seen(state, call_site->name());
    }

    state->vm()->metrics().machine.methods_invoked++;

    call_site->update(state, recv_class, dispatch);

    Executable* meth = dispatch.method;
    Module* mod = dispatch.module;

    return meth->execute(state, meth, mod, args);
  }

  void CallSite::empty_cache_updater(STATE, CallSite* call_site, Class* klass, Dispatch& dispatch) {
    MonoInlineCache* cache = MonoInlineCache::create(state, call_site, klass, dispatch);
    call_site->update_call_site(state, cache);
  }

  bool CallSite::update_and_validate(STATE, Object* recv, Symbol* vis, int serial) {
    Class* const recv_class = recv->direct_class(state);

    if(MonoInlineCache* mono = try_as<MonoInlineCache>(this)) {
      if(recv_class->data_raw() == mono->receiver_data_raw()) {
        return mono->method()->serial()->to_native() == serial;
      }
    }

    if(PolyInlineCache* cache = try_as<PolyInlineCache>(this)) {
      InlineCacheEntry* ice = cache->get_entry(recv_class);
      if(likely(ice)) return ice->method()->serial()->to_native() == serial;
    }

    LookupData lookup(state->vm()->call_frame()->self(),
        recv->lookup_begin(state), G(sym_public));
    Dispatch dispatch(name());

    if(dispatch.resolve(state, name(), lookup)) {
      update(state, recv_class, dispatch);
      return dispatch.method->serial()->to_native() == serial;
    }
    return false;
  }

  bool CallSite::lookup_method_missing(STATE,
      Arguments& args, Dispatch& dispatch, Object* self, Module* begin)
  {
    LookupData missing_lookup(self, begin, G(sym_private));
    Dispatch missing_dispatch(G(sym_method_missing));

    if(!missing_dispatch.resolve(state, G(sym_method_missing), missing_lookup)) {
      std::ostringstream msg;
      msg << "no method_missing for ";
      msg << begin->to_string(state);
      msg << "#" << dispatch.name->to_string(state);

      Exception::internal_error(state, msg.str().c_str());
      return false;
    }

    args.unshift(state, dispatch.name);
    dispatch.method = missing_dispatch.method;
    dispatch.module = missing_dispatch.module;
    state->vm()->set_method_missing_reason(dispatch.method_missing);
    state->vm()->global_cache()->add_seen(state, dispatch.name);

    return true;
  }

  void CallSite::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);
  }

}
