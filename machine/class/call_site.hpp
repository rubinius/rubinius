#ifndef RBX_BUILTIN_CALL_SITE_HPP
#define RBX_BUILTIN_CALL_SITE_HPP

#include "arguments.hpp"
#include "call_frame.hpp"
#include "dispatch.hpp"
#include "lookup_data.hpp"
#include "object_utils.hpp"
#include "spinlock.hpp"

#include "class/integer.hpp"
#include "class/object.hpp"
#include "class/prediction.hpp"
#include "class/symbol.hpp"

#include "diagnostics/machine.hpp"

#include <atomic>
#include <stdint.h>

namespace rubinius {
  class Dispatch;

  class CallSite : public Object {
    struct Cache {
      struct Entry {
        attr_field(hits, uint32_t);
        attr_field(misses, uint32_t);
        attr_field(receiver_class, Class*);
        attr_field(prediction, Prediction*);
        attr_field(module, Module*);
        attr_field(executable, Executable*);

        Entry(Class* receiver, Dispatch& dispatch)
          : _hits_(1)
          , _misses_(0)
          , _receiver_class_(receiver)
          , _prediction_(dispatch.prediction)
          , _module_(dispatch.module)
          , _executable_(dispatch.method)
        {
        }

        void hit() {
          ++_hits_;
        }

        void miss() {
          ++_misses_;
        }

        double efficiency() {
          double h = _hits_;
          double m = _misses_;

          return (h / (h + m) * h);
        }
      };

      attr_field(size, uint32_t);
      attr_field(hits, uint32_t);
      attr_field(misses, uint32_t);
      attr_field(evictions, uint32_t);

      Entry _entries_[0];

    public:
      Cache(uint32_t size)
        : _size_(size)
        , _hits_(0)
        , _misses_(0)
        , _evictions_(0)
      {
      }

      Cache(uint32_t size, Cache* cache)
        : _size_(size)
        , _hits_(cache->hits() + 1)
        , _misses_(cache->misses())
        , _evictions_(cache->evictions())
      {
      }

      static Cache* create(STATE, Class* receiver, Dispatch& dispatch) {
        uint8_t* mem = new uint8_t[sizeof(Cache) + sizeof(Entry)];

        Cache* new_cache = new(mem) Cache(1);
        new(new_cache->_entries_) Entry(receiver, dispatch);

        state->metrics()->inline_cache_count++;

        return new_cache;
      }

      static Cache* create(STATE, Cache* cache, Class* receiver, Dispatch& dispatch) {
        for(int32_t i = 0; i < cache->size(); i++) {
          if(cache->entries(i)->receiver_class() == receiver) {
            return cache;
          }
        }

        uint32_t size = cache->valid_size() + 1;

        uint8_t* mem = new uint8_t[sizeof(Cache) + (size * sizeof(Entry))];

        Cache* new_cache = new(mem) Cache(size, cache);
        new(new_cache->_entries_) Entry(receiver, dispatch);

        new_cache->copy_valid(cache, 1);

        state->metrics()->inline_cache_count++;

        return new_cache;
      }

      uint32_t valid_size() {
        uint32_t size = 0;

        for(int32_t i = 0; i < this->size(); i++) {
          if(entries(i)->prediction()->valid()) ++size;
        }

        if(size > max_caches) size = max_caches;

        return size;
      }

      void copy_valid(Cache* other, uint32_t index) {
        Entry* entries = other->entries();

        for(int32_t i = 0; i < other->size() && index < size(); i++) {
          if(entries[i].prediction()->valid()) {
            _entries_[index++] = entries[i];
          }
        }
      }

      Cache* compact() {
        uint32_t size = valid_size();

        if(size == this->size()) {
          return this;
        } else if(size == 0) {
          return nullptr;
        }

        uint8_t* mem = new uint8_t[sizeof(Cache) + (size * sizeof(Entry))];

        Cache* new_cache = new(mem) Cache(size, this);

        new_cache->copy_valid(this, 0);

        return new_cache;
      }

      void swap(uint32_t a, uint32_t b) {
        Entry tmp = _entries_[a];
        _entries_[a] = _entries_[b];
        _entries_[b] = tmp;
      }

      void reorder() {
        while(true) {
          bool swapped = false;

          for(uint32_t i = 0; i < size() - 1; i++) {
            if(entries(i)->efficiency() < entries(i+1)->efficiency()) {
              swap(i, i+1);
              swapped = true;
            }
          }

          if(!swapped) break;
        }
      }

      void hit() {
        ++_hits_;
      }

      void miss() {
        ++_misses_;
      }

      void evict(int num=1) {
        _evictions_ += num;
      }

      Entry* entries() {
        return _entries_;
      }

      Entry* entries(int i) {
        return _entries_ + i;
      }

      static Object* mono_execute(STATE, CallSite* call_site, Arguments& args) {
        Cache* cache = call_site->cache();
        Entry* entry = cache->entries();

        uint64_t klass = reinterpret_cast<uint64_t>(args.recv()->direct_class(state));
        uint64_t receiver = reinterpret_cast<uint64_t>(entry->receiver_class());

        if(likely((entry->prediction()->valid() & receiver) == klass)) {
          call_site->invoked();
          cache->hit();
          entry->hit();

          return entry->executable()->execute(state,
              entry->executable(), entry->module(), args);
        }

        cache->miss();
        entry->miss();

        return call_site->dispatch_and_cache(state, call_site, args);
      }

      static Object* poly_execute(STATE, CallSite* call_site, Arguments& args) {
        uint64_t klass = reinterpret_cast<uint64_t>(args.recv()->direct_class(state));

        Cache* cache = call_site->cache();
        Entry* entries = cache->entries();

        for(int i = 0; i < cache->size(); i++) {
          uint64_t receiver = reinterpret_cast<uint64_t>(entries[i].receiver_class());

          if((entries[i].prediction()->valid() & receiver) == klass) {
            call_site->invoked();
            cache->hit();
            entries[i].hit();

            return entries[i].executable()->execute(state,
                entries[i].executable(), entries[i].module(), args);
          } else {
            entries[i].miss();
          }
        }

        cache->miss();

        return call_site->dispatch_and_cache(state, call_site, args);
      }
    };

  public:
    const static object_type type = CallSiteType;

    static int memory_size;
    static int memory_words;
    static int max_caches;
    static int max_evictions;

    typedef Object* (Function)(STATE, CallSite*, Arguments&);
    typedef Function* Executor;

    attr_accessor(name, Symbol);

    attr_field(serial, uint64_t);
    attr_field(ip, int);
    attr_field(invokes, int);
    attr_field(rotations, int);
    attr_field(kind, MethodMissingReason);

    attr_field(executor, Executor);

    std::list<Cache*>* _dead_list_;
    locks::spinlock_mutex _dead_list_mutex_;

    std::atomic<Cache*> _cache_;

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, CallSite* obj) {
      obj->name(nil<Symbol>());
      obj->ip(0);
      obj->invokes(0);
      obj->rotations(0);
      obj->kind(eNone);
      obj->executor(CallSite::dispatch_once);

      obj->_dead_list_ = nullptr;
      obj->_dead_list_mutex_.unlock();

      obj->_cache_ = nullptr;
    }

    static CallSite* create(STATE, Symbol* name, uint64_t serial, int ip) {
      CallSite* cache = state->memory()->new_object<CallSite>(state, G(call_site));

      cache->name(name);
      cache->serial(serial);
      cache->ip(ip);

      state->metrics()->call_site_count++;

      return cache;
    }

    void set_cache(Cache* cache) {
      _cache_ = cache;
    }

    void delete_cache(Cache* cache) {
      delete[] reinterpret_cast<uint8_t*>(cache);
    }

    void invoked() {
      ++_invokes_;
    }

    void rotate() {
      ++_rotations_;
    }

    Cache* cache() {
      return _cache_;
    }

    Object* execute(STATE, Arguments& args) {
      return executor()(state, this, args);
    }

    void lookup(STATE, Arguments& args, Dispatch& dispatch, LookupData& lookup_data) {
      lookup_data.recv = state->call_frame()->self();

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
          lookup_data.from = state->call_frame()->module()->superclass();

          Symbol* original_name = state->call_frame()->original_name();
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
        state->set_method_missing_reason(dispatch.method_missing);
      }
    }

    void cache_method(STATE, Class* receiver, Dispatch& dispatch) {
      if(Cache* cache = this->cache()) {
        Cache* new_cache =
          Cache::create(state, cache, receiver, dispatch);

        if(new_cache == cache) return;

        if(_cache_.compare_exchange_strong(cache, new_cache)) {
          int diff;

          if((diff = (cache->size() - new_cache->size())) > 0) {
            new_cache->evict(diff);
          }

          rotate();
          dead_cache(cache);

          if(new_cache->size() == 1) {
            executor(Cache::mono_execute);
          } else {
            executor(Cache::poly_execute);
          }

          std::atomic_thread_fence(std::memory_order_seq_cst);
        } else {
          delete_cache(new_cache);
        }
      } else {
        Cache* new_cache =
          Cache::create(state, receiver, dispatch);

        if(_cache_.compare_exchange_strong(cache, new_cache)) {
          executor(Cache::mono_execute);

          std::atomic_thread_fence(std::memory_order_seq_cst);
        } else {
          delete_cache(new_cache);
        }
      }
    }

    bool valid_serial_p(STATE, Object* recv, Symbol* vis, int serial) {
      if(Cache* cache = this->cache()) {
        uint64_t klass = reinterpret_cast<uint64_t>(recv->direct_class(state));

        for(int i = 0; i < cache->size(); i++) {
          Cache::Entry* entry = cache->entries(i);

          uint64_t receiver = reinterpret_cast<uint64_t>(entry->receiver_class());

          if(((entry->prediction()->valid() & receiver) == klass)
              && (entry->executable()->serial()->to_native() == serial))
          {
            return true;
          }
        }
      }

      Dispatch dispatch(name());
      LookupData lookup_data(state->call_frame()->self(),
          recv->lookup_begin(state), vis);

      if(dispatch.resolve(state, name(), lookup_data)) {
        if(dispatch.method->serial()->to_native() == serial) {
          cache_method(state, recv->direct_class(state), dispatch);

          return true;
        }
      }

      return false;
    }

    static Object* dispatch(STATE, CallSite* call_site, Arguments& args) {
      call_site->invoked();

      Dispatch dispatch(call_site->name());
      LookupData lookup_data;

      call_site->lookup(state, args, dispatch, lookup_data);

      return dispatch.method->execute(state, dispatch.method, dispatch.module, args);
    }

    static Object* dispatch_and_cache(STATE, CallSite* call_site, Arguments& args) {
      call_site->invoked();

      Dispatch dispatch(call_site->name());
      LookupData lookup_data;

      call_site->lookup(state, args, dispatch, lookup_data);

      Object* value = nullptr;

      if((value = dispatch.method->execute(state, dispatch.method, dispatch.module, args))) {
        if(dispatch.name == G(sym_method_missing)) return value;

        call_site->cache_method(state, args.recv()->direct_class(state), dispatch);
      }

      return value;
    }

    static Object* dispatch_once(STATE, CallSite* call_site, Arguments& args) {
      call_site->invoked();

      Dispatch dispatch(call_site->name());
      LookupData lookup_data;

      call_site->lookup(state, args, dispatch, lookup_data);

      Object* value = nullptr;

      if((value = dispatch.method->execute(state, dispatch.method, dispatch.module, args))) {
        if(dispatch.name == G(sym_method_missing)) return value;

        if(call_site->executor() == CallSite::dispatch_once) {
          call_site->executor(CallSite::dispatch_and_cache);
        }
      }

      return value;
    }

    void dead_cache(Cache* cache) {
      std::lock_guard<locks::spinlock_mutex> guard(_dead_list_mutex_);

      if(cache) {
        if(!_dead_list_) {
          _dead_list_ = new std::list<Cache*>();
        }

        _dead_list_->push_back(cache);
      }
    }

    void clear_dead_list() {
      if(_dead_list_) {
        while(!_dead_list_->empty()) {
          Cache* cache = _dead_list_->back();
          _dead_list_->pop_back();
          delete_cache(cache);
        }
      }
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

      if(Cache* cache = this->cache()) {
        delete_cache(cache);
        _cache_ = nullptr;
      }
    }

    // Rubinius.primitive :call_site_ip
    Fixnum* ip(STATE) {
      return Fixnum::from(ip());
    }

    // Rubinius.primitive :call_site_size
    Fixnum* size(STATE) {
      Cache* cache = this->cache();

      return Fixnum::from(cache ? cache->size() : 0);
    }

    // Rubinius.primitive :call_site_invokes
    Fixnum* invokes(STATE) {
      return Fixnum::from(invokes());
    }

    // Rubinius.primitive :call_site_evictions
    Fixnum* evictions(STATE) {
      Cache* cache = this->cache();

      return Fixnum::from(cache ? cache->evictions() : 0);
    }

    // Rubinius.primitive :call_site_rotations
    Fixnum* rotations(STATE) {
      return Fixnum::from(rotations());
    }

    // Rubinius.primitive :call_site_hits
    Fixnum* hits(STATE) {
      Cache* cache = this->cache();

      return Fixnum::from(cache ? cache->hits() : 0);
    }

    // Rubinius.primitive :call_site_misses
    Fixnum* misses(STATE) {
      Cache* cache = this->cache();

      return Fixnum::from(cache ? cache->misses() : 0);
    }

    // Rubinius.primitive :call_site_dead_list_size
    Fixnum* dead_list_size(STATE) {
      return Fixnum::from(_dead_list_ ? _dead_list_->size() : 0);
    }

    // Rubinius.primitive :call_site_cache_entries
    Tuple* cache_entries(STATE);

    // Rubinius.primitive :call_site_reset
    CallSite* reset(STATE) {
      invokes(0);
      executor(CallSite::dispatch_once);

      if(Cache* cache = this->cache()) {
        delete_cache(cache);
        _cache_ = nullptr;
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

      virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void set_field(STATE, Object* target, size_t index, Object* val);
      virtual Object* get_field(STATE, Object* target, size_t index);
      virtual void before_visit(STATE, Object* o, std::function<void (STATE, Object**)> f);
      virtual void after_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {}
      virtual void visit(STATE, Object* o, std::function<void (STATE, Object**)> f);
      virtual void populate_slot_locations();
    };
  };
}
#endif
