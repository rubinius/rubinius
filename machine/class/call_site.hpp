#ifndef RBX_BUILTIN_CALL_SITE_HPP
#define RBX_BUILTIN_CALL_SITE_HPP

#include "arguments.hpp"
#include "call_frame.hpp"
#include "dispatch.hpp"
#include "global_cache.hpp"
#include "lookup_data.hpp"
#include "object_utils.hpp"
#include "vm.hpp"

#include "class/integer.hpp"
#include "class/object.hpp"
#include "class/symbol.hpp"

#include "util/atomic.hpp"

#include <atomic>
#include <stdint.h>

namespace rubinius {
  class Dispatch;

  class CallSite : public Object {
  public:
    struct InlineCache;

    const static object_type type = CallSiteType;

    typedef Object* (Function)(STATE, CallSite*, Arguments&);

    typedef Function* Executor;

    attr_accessor(name, Symbol);

    attr_field(invokes, int);
    attr_field(hits, int);
    attr_field(misses, int);
    attr_field(evictions, int);
    attr_field(ip, int);
    attr_field(kind, MethodMissingReason);

    attr_field(execute, Executor);
    attr_field(cache_miss, Executor);

    std::list<InlineCache*>* _dead_list_;
    locks::spinlock_mutex _dead_list_mutex_;

    typedef std::atomic<InlineCache*> InlineCachePtr;

    InlineCachePtr* _caches_;

  public:
    static int max_caches;
    static int max_evictions;
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

      InlineCache(InlineCache* cache, Dispatch& dispatch, ClassData class_data)
        : _receiver_class_(cache->receiver_class())
        , _stored_module_(dispatch.module)
        , _executable_(dispatch.method)
        , _receiver_data_(class_data)
        , _method_missing_(cache->method_missing())
        , _hits_(0)
        , _misses_(0)
      {
        if(dispatch.method_missing == eNone) {
          execute(invoke);
        } else {
          execute(invoke_method_missing);
        }
      }

      InlineCache(Class* klass, Dispatch& dispatch)
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
          call_site->hit();
          hit();
          valid_p = true;
          return _execute_(state, call_site, this, args);
        }

        call_site->miss();
        miss();

        return nullptr;
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

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, CallSite* obj) {
      obj->name(nil<Symbol>());
      obj->invokes(0);
      obj->hits(0);
      obj->misses(0);
      obj->evictions(0);
      obj->ip(0);
      obj->kind(eNone);
      obj->execute(default_execute);
      obj->cache_miss(default_execute);

      obj->_dead_list_ = new std::list<InlineCache*>();
      obj->_dead_list_mutex_.unlock();

      obj->_caches_ = new InlineCachePtr[max_caches];

      for(int i = 0; i < max_caches; i++) {
        obj->_caches_[i] = nullptr;
      }
    }

    void dead_cache(InlineCache* cache) {
      std::lock_guard<locks::spinlock_mutex> guard(_dead_list_mutex_);
      if(cache) _dead_list_->push_back(cache);
    }

    void clear_dead_list() {
      while(!_dead_list_->empty()) {
        InlineCache* cache = _dead_list_->back();
        _dead_list_->pop_back();
        delete cache;
      }
    }

    void hit() {
      ++_hits_;
    }

    void miss() {
      ++_misses_;
    }

    void evict() {
      ++_evictions_;
    }

    int depth() {
      for(int i = 0; i < max_caches; i++) {
        if(!_caches_[i]) return i;
      }

      return max_caches;
    }

    void invoked() {
      ++_invokes_;
    }

    bool valid_serial_p(STATE, Object* recv, Symbol* vis, int serial) {
      Class* recv_class = recv->direct_class(state);
      uint64_t class_data_raw = recv_class->data_raw();

      for(int i = 0; i < max_caches; i++) {
        if(InlineCache* cache = _caches_[i]) {
          if(cache->valid_serial_p(class_data_raw, serial)) {
            return true;
          }
        } else {
          break;
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

    void finalize(STATE) {
      if(_dead_list_) {
        clear_dead_list();

        delete _dead_list_;
        _dead_list_ = nullptr;
      }

      for(int i = 0; i < max_caches; i++) {
        if(InlineCache* cache = _caches_[i]) {
          delete cache;
          _caches_[i] = nullptr;
        }
      }

      delete[] _caches_;
    }

    static CallSite* create(STATE, Symbol* name, int ip) {
      CallSite* cache =
        state->memory()->new_variable_object<CallSite>(state, G(call_site));

      cache->name(name);
      cache->ip(ip);

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

      return nullptr;
    }

    static Object* invoke_cached(STATE, CallSite* call_site, Arguments& args) {
      bool valid_p = false;
      InlineCache* cache = 0;

      for(int i = 0; i < max_caches && (cache = call_site->_caches_[i]); i++) {
        Object* value = cache->execute(state, call_site, args, valid_p);

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

      return nullptr;
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
      InlineCache* cache = 0;

      // 1. Attempt to update a cache.
      for(int i = 0; i < max_caches && (cache = _caches_[i]); i++) {
        // We know that nothing matched the cache, so we only need this test
        if(cache->receiver_data().class_id() == class_data.class_id()) {
          InlineCache* new_cache = new InlineCache(cache, dispatch, class_data);
          InlineCache* old_cache = cache;

          if(_caches_[i].compare_exchange_strong(cache, new_cache)) {
            dead_cache(old_cache);
            state->vm()->metrics().machine.inline_cache_updated++;
            return;
          } else {
            delete new_cache;
          }
        }
      }

      // 2. Attempt to replace a cache.
      for(int i = 0; i < max_caches && (cache = _caches_[i]); i++) {
        if(cache->inefficient_p()) {
          InlineCache* new_cache = new InlineCache(klass, dispatch);
          InlineCache* old_cache = cache;

          if(_caches_[i].compare_exchange_strong(cache, new_cache)) {
            dead_cache(old_cache);
            state->vm()->metrics().machine.inline_cache_replaced++;
            return;
          } else {
            delete new_cache;
          }
        }
      }

      // 3. Attempt to add a cache
      for(int i = 0; i < max_caches; i++) {
        InlineCache* cache = _caches_[i];

        if(cache) continue;

        InlineCache* new_cache = new InlineCache(klass, dispatch);

        if(_caches_[i].compare_exchange_strong(cache, new_cache)) {
          if(i == 0) {
            state->vm()->metrics().machine.inline_cache_count++;
          } else {
            state->vm()->metrics().machine.call_site_polymorphic++;
          }

          execute(invoke_cached);

          return;
        } else {
          delete new_cache;
        }
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
      return _cache_miss_(state, this, args);
    }

    void evict_and_mark(memory::ObjectMark& mark) {
      // 0. Evict inefficient caches.
      for(int i = 0; i < max_caches; i++) {
        if(InlineCache* cache = _caches_[i]) {
          if(cache->inefficient_p()) {
            evict();

            delete cache;
            _caches_[i] = nullptr;

            // TODO: pass State into GC!
            VM::current()->metrics().machine.inline_cache_evicted++;
          }
        }
      }

      // 1. Compact if there were evictions.
      for(int i = 0; i < max_caches - 1; i++) {
        InlineCache* a = _caches_[i];
        InlineCache* b = _caches_[i+1];

        if(a == nullptr && b != nullptr) {
          _caches_[i] = b;
          _caches_[i+1] = nullptr;

          // TODO: pass State into GC!
          VM::current()->metrics().machine.inline_cache_reordered++;
        }
      }

      // 2. Check if all caching should be disabled.
      if(depth() == 0 && evictions() > max_evictions) {
        execute(CallSite::dispatch);
        cache_miss(CallSite::dispatch);

        // TODO: pass State into GC!
        VM::current()->metrics().machine.inline_cache_disabled++;
      }

      // 3. Mark remaining caches.
      for(int i = 0; i < max_caches; i++) {
        if(InlineCache* cache = _caches_[i]) {
          if(Object* ref = mark.call(cache->receiver_class())) {
            cache->receiver_class(as<Class>(ref));
            mark.just_set(this, ref);
          }

          if(Object* ref = mark.call(cache->stored_module())) {
            cache->stored_module(as<Module>(ref));
            mark.just_set(this, ref);
          }

          if(Object* ref = mark.call(cache->executable())) {
            cache->executable(as<Executable>(ref));
            mark.just_set(this, ref);
          }
        }
      }

      // 4. Clear dead list
      clear_dead_list();
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
      invokes(0);
      execute(default_execute);
      cache_miss(default_execute);

      for(int i = 0; i < max_caches; i++) {
        if(InlineCache* cache = _caches_[i]) {
          _caches_[i] = nullptr;
          dead_cache(cache);
        }
      }

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
