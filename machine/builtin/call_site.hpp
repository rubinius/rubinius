#ifndef RBX_BUILTIN_CALL_SITE_HPP
#define RBX_BUILTIN_CALL_SITE_HPP

#include "arguments.hpp"
#include "call_frame.hpp"
#include "dispatch.hpp"
#include "global_cache.hpp"
#include "lookup_data.hpp"
#include "object_utils.hpp"
#include "vm.hpp"

#include "builtin/integer.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"

#include "util/atomic.hpp"

#include <stdint.h>

namespace rubinius {
  class Dispatch;

  class CallSite : public Object {
  public:
    struct InlineCache;
    struct InlineCaches;

    const static object_type type = CallSiteType;

    typedef Object* (Function)(STATE, CallSite*, Arguments&);

    typedef Function* Executor;

    attr_accessor(name, Symbol);

    attr_field(invokes, int);
    attr_field(ip, int);
    attr_field(kind, MethodMissingReason);

    attr_field(execute, Executor);
    attr_field(cache_miss, Executor);

    attr_field(caches, InlineCaches*);

  public:
    static int max_caches;
    static Executor default_execute;

    struct InlineCache {
      typedef Object* (Function)(STATE, CallSite*, InlineCache*, Arguments&);
      typedef Function* Executor;

      attr_field(receiver_class, Class*);
      attr_field(stored_module, Module*);
      attr_field(executable, Executable*);

      attr_field(receiver_data, ClassData);
      attr_field(method_missing, MethodMissingReason);
      attr_field(hits, int);
      attr_field(misses, int);

      attr_field(execute, Executor);

      InlineCache() { };

      InlineCache(STATE, Class* klass, Dispatch& dispatch)
        : _receiver_class_(klass)
        , _stored_module_(dispatch.module)
        , _executable_(dispatch.method)
        , _receiver_data_(klass->class_data())
        , _method_missing_(dispatch.method_missing)
        , _hits_(0)
        , _misses_(0)
      {
        if(dispatch.method_missing == eNone) {
          execute(invoke);
        } else {
          execute(invoke_method_missing);
        }
      }

      void update(Class* klass, Dispatch& dispatch) {
        receiver_class(klass);
        stored_module(dispatch.module);
        executable(dispatch.method);
        receiver_data(klass->class_data());
        method_missing(dispatch.method_missing);
      }

      void update(Dispatch& dispatch, ClassData& class_data) {
        stored_module(dispatch.module);
        executable(dispatch.method);
        receiver_data(class_data);
      }

      void hit() {
        ++_hits_;
      }

      void miss() {
        ++_misses_;
      }

      bool inefficient_p() {
        return misses() > hits();
      }

      bool valid_serial_p(uint64_t class_data_raw, int serial) {
        return class_data_raw == receiver_data().raw
          && executable()->serial()->to_native() == serial;
      }

      Object* execute(STATE, CallSite* call_site, Arguments& args, bool& valid_p) {
        uint64_t raw_data = args.recv()->direct_class(state)->data_raw();

        if(likely(receiver_data().raw == raw_data)) {
          hit();
          valid_p = true;
          return _execute_(state, call_site, this, args);
        }

        miss();

        return NULL;
      }

      static Object* invoke(STATE, CallSite* call_site,
          InlineCache* cache, Arguments& args)
      {
        state->vm()->metrics().machine.methods_invoked++;
        state->vm()->metrics().machine.inline_cache_hits++;

        return cache->executable()->execute(state,
            cache->executable(), cache->stored_module(), args);
      }

      static Object* invoke_method_missing(STATE, CallSite* call_site,
          InlineCache* cache, Arguments& args)
      {
        state->vm()->metrics().machine.methods_invoked++;
        state->vm()->metrics().machine.inline_cache_hits++;

        state->vm()->set_method_missing_reason(cache->method_missing());
        args.unshift(state, call_site->name());

        return cache->executable()->execute(state, cache->executable(),
            cache->stored_module(), args);
      }
    };

    struct InlineCaches {
      attr_field(depth, int);

      InlineCache cache[0];

      static InlineCaches* allocate(int size) {
        return static_cast<InlineCaches*>(
            malloc(sizeof(int) + size * sizeof(InlineCache)));
      }
    };

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, CallSite* obj) {
      obj->name(nil<Symbol>());
      obj->invokes(0);
      obj->ip(0);
      obj->kind(eNone);
      obj->execute(default_execute);
      obj->cache_miss(default_execute);
      obj->caches(NULL);
    }

    static void finalize(STATE, CallSite* call_site) {
      if(call_site->caches()) free(call_site->caches());
    }

    static CallSite* create(STATE, Symbol* name, int ip) {
      CallSite* cache = state->memory()->new_object<CallSite>(state, G(call_site));

      cache->name(name);
      cache->ip(ip);

      state->memory()->native_finalizer(state, cache,
          (memory::FinalizerFunction)&CallSite::finalize);

      state->vm()->metrics().machine.call_site_count++;

      return cache;
    }

    static Object* lookup_invoke_cache(STATE, CallSite* call_site, Arguments& args) {
      Dispatch dispatch(call_site->name());
      LookupData lookup_data;

      if(call_site->lookup(state, args, dispatch, lookup_data)) {
        if(Object* value = call_site->invoke(state, args, dispatch)) {
          call_site->cache(state, args.recv()->direct_class(state), dispatch);

          return value;
        }
      }

      return NULL;
    }

    static Object* invoke_cached(STATE, CallSite* call_site, Arguments& args) {
      bool valid_p = false;

      for(int i = 0; i < call_site->depth(); i++) {
        Object* value =
          call_site->caches()->cache[i].execute(state, call_site, args, valid_p);

        if(valid_p) return value;
      }

      return call_site->cache_miss(state, args);
    }

    static Object* dispatch(STATE, CallSite* call_site, Arguments& args) {
      Dispatch dispatch(call_site->name());
      LookupData lookup_data;

      if(call_site->lookup(state, args, dispatch, lookup_data)) {
        return call_site->invoke(state, args, dispatch);
      }

      return NULL;
    }

    Object* invoke(STATE, Arguments& args, Dispatch& dispatch) {
      Executable* meth = dispatch.method;
      Module* mod = dispatch.module;

      invoked();

      state->vm()->metrics().machine.methods_invoked++;
      state->vm()->metrics().machine.inline_cache_misses++;

      return meth->execute(state, meth, mod, args);
    }

    bool lookup(STATE, Arguments& args, Dispatch& dispatch, LookupData& lookup_data) {
      // TODO: why isn't this 'args.recv()'?
      lookup_data.recv = state->vm()->call_frame()->self();

      switch(kind()) {
        case eNone:
        case eNormal:
          lookup_data.from = args.recv()->lookup_begin(state);
          lookup_data.min_visibility = G(sym_public);
          break;
        case eVCall:
        case ePrivate:
        case eProtected:
          lookup_data.min_visibility = G(sym_private);
          lookup_data.from = args.recv()->lookup_begin(state);
          break;
        case eSuper: {
          lookup_data.min_visibility = G(sym_private);
          lookup_data.from = state->vm()->call_frame()->module()->superclass();

          Symbol* original_name = state->vm()->call_frame()->original_name();
          if(name() != original_name) {
            name(original_name);
            args.set_name(name());
          }

          break;
        }
      }

      if(lookup_data.from->nil_p() || !dispatch.resolve(state, name(), lookup_data)) {
        Module* from = kind() == eSuper ? args.recv()->lookup_begin(state) : lookup_data.from;
        LookupData missing_lookup(lookup_data.recv, from, G(sym_private));
        Dispatch missing_dispatch(G(sym_method_missing));

        if(!missing_dispatch.resolve(state, G(sym_method_missing), missing_lookup)) {
          Exception::raise_no_method_error(state, args);
        }

        args.unshift(state, dispatch.name);
        dispatch.name = missing_dispatch.name;
        dispatch.method = missing_dispatch.method;
        dispatch.module = missing_dispatch.module;
        dispatch.method_missing = kind();
        state->vm()->set_method_missing_reason(dispatch.method_missing);
        state->vm()->global_cache()->add_seen(state, dispatch.name);
      }

      return true;
    }

    void cache(STATE, Class* klass, Dispatch& dispatch) {
      // 0. Ignore method_missing for now
      if(dispatch.name == G(sym_method_missing)) return;

      ClassData class_data = klass->class_data();

      // 1. Attempt to update a cache.
      for(int i = 0; i < depth(); i++) {
        InlineCache* cache = &caches()->cache[i];

        // We know that nothing matched the cache, so we only need this test
        if(cache->receiver_data().class_id() == class_data.class_id()) {
          cache->update(dispatch, class_data);
          atomic::memory_barrier();

          state->vm()->metrics().machine.inline_cache_updated++;

          return;
        }
      }

      // 2. Attempt to replace a cache.
      for(int i = 0; i < depth(); i++) {
        InlineCache* cache = &caches()->cache[i];

        if(cache->inefficient_p()) {
          cache->update(klass, dispatch);
          atomic::memory_barrier();

          state->vm()->metrics().machine.inline_cache_replaced++;

          return;
        }
      }

      // 3. Attempt to add a cache
      if(depth() < max_caches) {
        InlineCaches* inline_caches = 0;
        int size = depth() + 1;

        if(!caches()) {
          inline_caches = InlineCaches::allocate(1);
          execute(invoke_cached);
        } else {
          inline_caches = InlineCaches::allocate(CallSite::max_caches);
          state->vm()->metrics().machine.call_site_polymorphic++;
        }

        inline_caches->depth(size);
        new(inline_caches->cache) InlineCache(state, klass, dispatch);

        state->vm()->metrics().machine.inline_cache_count++;

        for(int i = 1; i < size; i++) {
          inline_caches->cache[i] = caches()->cache[i - 1];
        }

        /* We CAS here because under concurrency, we don't know who may have
         * beat us here and whether those caches are in use. We know that our
         * caches are not in use, so if we fail to CAS, we simply discard our
         * work and eventually the executable will cache again if it is used.
         */
        InlineCaches* previous_caches = caches();
        InlineCaches** updated_caches = &_caches_;
        if(atomic::compare_and_swap(reinterpret_cast<void**>(updated_caches),
            previous_caches, inline_caches))
        {
          if(previous_caches) delete previous_caches;
        } else {
          delete inline_caches;
        }

        return;
      }

      // 4. Check if we should stop trying to cache.
      if(invokes() > CallSite::max_caches) {
        state->vm()->metrics().machine.call_site_full++;
        cache_miss(CallSite::dispatch);
      }
    }

    Object* execute(STATE, Arguments& args) {
      return _execute_(state, this, args);
    }

    Object* cache_miss(STATE, Arguments& args) {
      Object* value = _cache_miss_(state, this, args);

      // Check if all caching should be disabled.
      if(invokes() > CallSite::max_caches) {
        if(depth() == CallSite::max_caches) {
          int disable = 0;

          for(int i = 0; i < depth(); i++) {
            InlineCache* cache = &caches()->cache[i];

            if(cache->inefficient_p()) {
              disable++;
            }
          }

          if(disable == depth()) {
            execute(CallSite::dispatch);
            cache_miss(CallSite::dispatch);

            delete caches();
            caches(NULL);

            atomic::memory_barrier();

            state->vm()->metrics().machine.inline_cache_disabled++;

            return value;
          }
        }
      }

      return value;
    }

    void invoked() {
      ++_invokes_;
    }

    int depth() {
      if(caches()) {
        return caches()->depth();
      } else {
        return 0;
      }
    }

    int hits() {
      int hits = 0;

      for(int i = 0; i < depth(); i++) {
        hits += caches()->cache[i].hits();
      }

      return hits;
    }

    int misses() {
      int misses = 0;

      for(int i = 0; i < depth(); i++) {
        misses += caches()->cache[i].misses();
      }

      return misses;
    }

    bool valid_serial_p(STATE, Object* recv, Symbol* vis, int serial) {
      Class* recv_class = recv->direct_class(state);
      uint64_t class_data_raw = recv_class->data_raw();

      for(int i = 0; i < depth(); i++) {
        if(caches()->cache[i].valid_serial_p(class_data_raw, serial)) {
          return true;
        }
      }

      Dispatch dispatch(name());
      LookupData lookup_data(state->vm()->call_frame()->self(),
          recv->lookup_begin(state), vis);

      if(dispatch.resolve(state, name(), lookup_data)) {
        if(dispatch.method->serial()->to_native() == serial) {
          cache(state, recv_class, dispatch);

          return true;
        }
      }

      return false;
    }

    void set_is_private() {
      kind(ePrivate);
    }

    void set_is_super() {
      kind(eSuper);
    }

    void set_is_vcall() {
      kind(eVCall);
    }

    // Rubinius.primitive :call_site_ip
    Integer* ip(STATE) {
      return Integer::from(state, ip());
    }

    // Rubinius.primitive :call_site_depth
    Fixnum* depth(STATE) {
      return Fixnum::from(depth());
    }

    // Rubinius.primitive :call_site_invokes
    Integer* invokes(STATE) {
      return Integer::from(state, invokes());
    }

    // Rubinius.primitive :call_site_hits
    Integer* hits(STATE) {
      return Integer::from(state, hits());
    }

    // Rubinius.primitive :call_site_misses
    Integer* misses(STATE) {
      return Integer::from(state, misses());
    }

    // Rubinius.primitive :call_site_reset
    CallSite* reset(STATE) {
      if(caches()) delete caches();

      invokes(0);
      execute(default_execute);
      cache_miss(default_execute);
      caches(NULL);

      return this;
    }

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(Object* obj, memory::ObjectMark& mark);
      virtual void auto_mark(Object* obj, memory::ObjectMark& mark);
      virtual void set_field(STATE, Object* target, size_t index, Object* val);
      virtual Object* get_field(STATE, Object* target, size_t index);
      virtual void populate_slot_locations();
    };
  };
}
#endif
