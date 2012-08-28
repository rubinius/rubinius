#ifndef RBX_INLINE_CACHE_HPP
#define RBX_INLINE_CACHE_HPP

#include "dispatch.hpp"
#include "builtin/object.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/cache.hpp"
#include "lock.hpp"
#include "object_utils.hpp"

#include <tr1/unordered_map>
#include <tr1/unordered_set>

// #define TRACK_IC_LOCATION

namespace rubinius {
  class InlineCache;
  struct CallFrame;
  class Arguments;
  class CallUnit;

  // How many receiver class have been seen to keep track of inside an IC
  const static int cTrackedICHits = 3;

  class InlineCacheHit {
    MethodCacheEntry* entry_;
    int hits_;

  public:

    InlineCacheHit()
      : hits_(0)
    {}

    void assign(MethodCacheEntry* entry) {
      hits_ = 0;
      atomic::write(&entry_, entry);
    }

    void hit() {
      ++hits_;
    }

    int hits() {
      return hits_;
    }

    MethodCacheEntry* entry() {
      return entry_;
    }
  };

  class InlineCache {
  public:
    Symbol* name;

  private:
    InlineCacheHit cache_[cTrackedICHits];

    CallUnit* call_unit_;

    typedef Object* (*CacheExecutor)(STATE, InlineCache*, CallFrame*, Arguments& args);

    CacheExecutor initial_backend_;
    CacheExecutor execute_backend_;

#ifdef TRACK_IC_LOCATION
    int ip_;
    MachineCode* machine_code_;
#endif

    int seen_classes_overflow_;

  public:

    friend class CompiledCode::Info;

    static Object* empty_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_private(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_vcall(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_custom(STATE, InlineCache* cache, CallFrame* call_frame,
                                          Arguments& args);

    static Object* check_cache_symbol(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_fixnum(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_reference(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);

    static Object* check_cache_custom(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_super_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);

    static Object* check_cache_poly(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);

    static Object* disabled_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);
    static Object* disabled_cache_private(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);
    static Object* disabled_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);

    MethodMissingReason fill_public(STATE, Object* self, Symbol* name, Class* klass,
                                    MethodCacheEntry*& mce, bool super = false);
    bool fill_private(STATE, Symbol* name, Module* start, Class* klass,
                      MethodCacheEntry*& mce, bool super = false);
    bool fill_method_missing(STATE, Class* klass, MethodMissingReason reason, MethodCacheEntry*& mce);

    MethodCacheEntry* update_and_validate(STATE, CallFrame* call_frame, Object* recv);
    MethodCacheEntry* update_and_validate_private(STATE, CallFrame* call_frame, Object* recv);

    InlineCache()
      : name(0)
      , call_unit_(0)
      , initial_backend_(empty_cache)
      , execute_backend_(empty_cache)
      , seen_classes_overflow_(0)
    {
      clear();
    }

#ifdef TRACK_IC_LOCATION
    void set_location(int ip, MachineCode* mcode) {
      ip_ = ip;
      machine_code_ = mcode;
    }

    int ip() {
      return ip_;
    }

    MachineCode* machine_code() {
      return machine_code_;
    }
#else
    void set_location(int ip, MachineCode* mcode) { }
#endif

    void print_location(STATE, std::ostream& stream);
    void print(STATE, std::ostream& stream);

    void set_name(Symbol* sym) {
      name = sym;
    }

    InlineCacheHit* caches() {
      return cache_;
    }

    void set_is_private() {
      initial_backend_ = empty_cache_private;
      execute_backend_ = empty_cache_private;
    }

    void set_is_super() {
      initial_backend_ = empty_cache_super;
      execute_backend_ = empty_cache_super;
    }

    void set_is_vcall() {
      initial_backend_ = empty_cache_vcall;
      execute_backend_ = empty_cache_vcall;
    }

    void set_call_custom() {
      initial_backend_ = empty_cache_custom;
      execute_backend_ = empty_cache_custom;
    }

    Object* execute(STATE, CallFrame* call_frame, Arguments& args) {
      return (*execute_backend_)(state, this, call_frame, args);
    }

    Object* initialize(STATE, CallFrame* call_frame, Arguments& args) {
      return (*initial_backend_)(state, this, call_frame, args);
    }

    void clear() {
      for(int i = 0; i < cTrackedICHits; ++i) {
        cache_[i].assign(NULL);
      }
    }

    MethodCacheEntry* get_cache(Object* const recv_class) {
      for(int i = 0; i < cTrackedICHits; ++i) {
        MethodCacheEntry* mce = cache_[i].entry();
        if(likely(mce && mce->receiver_class() == recv_class)) return mce;
      }
      return NULL;
    }

    InlineCacheHit* get_inline_cache(Object* const recv_class, MethodCacheEntry*& mce) {
      for(int i = 0; i < cTrackedICHits; ++i) {
        mce = cache_[i].entry();
        if(likely(mce && mce->receiver_class() == recv_class)) return &cache_[i];
      }
      return NULL;
    }

    MethodCacheEntry* get_single_cache() {
      if(cache_size() == 1) {
        return cache_[0].entry();
      }
      return NULL;
    }

    int cache_size() {
      for(int i = 0; i < cTrackedICHits; ++i) {
        if(!cache_[i].entry()) return i;
      }
      return cTrackedICHits;
    }

    void set_cache(MethodCacheEntry* mce) {
      // Make sure we sync here, so the MethodCacheEntry mce is
      // guaranteed completely initialized. Otherwise another thread
      // might see an incompletely initialized MethodCacheEntry.
      int least_used = cTrackedICHits - 1;
      for(int i = 0; i < cTrackedICHits; ++i) {
        if(!cache_[i].entry()) {
          cache_[i].assign(mce);
          return;
        }
        if(cache_[i].entry()->receiver_class() == mce->receiver_class()) return;
        if(cache_[i].hits() < cache_[least_used].hits()) {
          least_used = i;
        }
      }

      seen_classes_overflow_++;
      cache_[least_used].assign(mce);
    }

    int seen_classes_overflow() {
      return seen_classes_overflow_;
    }

    int classes_seen() {
      return cache_size();
    }

    Class* tracked_class(int which) {
      return cache_[which].entry()->receiver_class();
    }

    Class* find_class_by_id(int64_t id) {
      for(int i = 0; i < cTrackedICHits; i++) {
        if(cache_[i].entry()) {
          Class* cls = cache_[i].entry()->receiver_class();
          if(cls && cls->class_id() == id) return cls;
        }
      }

      return NULL;
    }

    Class* find_singletonclass(int64_t id) {
      for(int i = 0; i < cTrackedICHits; i++) {
        if(cache_[i].entry()) {
          if(Class* cls = cache_[i].entry()->receiver_class()) {
            if(SingletonClass* sc = try_as<SingletonClass>(cls)) {
              if(Class* ref = try_as<Class>(sc->attached_instance())) {
                if(ref->class_id() == id) return cls;
              }
            }
          }
        }
      }

      return NULL;
    }

    Class* get_class(int idx) {
       MethodCacheEntry* entry = cache_[idx].entry();
      if(entry) {
        return entry->receiver_class();
      } else {
        return NULL;
      }
    }

  };

  // Registry, used to clear ICs by method name
  class InlineCacheRegistry : public Lockable {
    typedef std::tr1::unordered_set<InlineCache*> CacheSet;
    typedef std::tr1::unordered_map<native_int, CacheSet> CacheHash;

    CacheHash caches_;

  public:
    void add_cache(STATE, Symbol* sym, InlineCache* cache);
    void remove_cache(STATE, Symbol* sym, InlineCache* cache);
    void clear(STATE, Symbol* sym);

    void print_stats(STATE);
  };
}

#endif
