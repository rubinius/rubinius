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
      : entry_(NULL)
      , hits_(0)
    {}

    void assign(MethodCacheEntry* entry) {
      hits_ = 0;
      atomic::write(&entry_, entry);
    }

    void update(MethodCacheEntry* entry) {
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

  class InlineCache : public Object {
  public:
    const static object_type type = InlineCacheType;

  private:
    Symbol* name_; // slot
    CallUnit* call_unit_; // slot

    InlineCacheHit cache_[cTrackedICHits];

    typedef Object* (*CacheExecutor)(STATE, InlineCache*, CallFrame*, Arguments& args);

    CacheExecutor initial_backend_;
    CacheExecutor execute_backend_;

#ifdef TRACK_IC_LOCATION
    int ip_;
    MachineCode* machine_code_;
#endif

    int seen_classes_overflow_;

  public:
    attr_accessor(name, Symbol);
    attr_accessor(call_unit, CallUnit);

    static void init(STATE);
    static InlineCache* empty(STATE, Symbol* name);

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

    MethodCacheEntry* get_cache(Class* const recv_class) {
      register uint64_t recv_data = recv_class->data_id();
      for(int i = 0; i < cTrackedICHits; ++i) {
        MethodCacheEntry* mce = cache_[i].entry();
        if(likely(mce && mce->receiver_data() == recv_data)) return mce;
      }
      return NULL;
    }

    InlineCacheHit* get_inline_cache(Class* const recv_class, MethodCacheEntry*& mce) {
      register uint64_t recv_data = recv_class->data_id();
      for(int i = 0; i < cTrackedICHits; ++i) {
        mce = cache_[i].entry();
        if(likely(mce && mce->receiver_data() == recv_data)) return &cache_[i];
      }
      return NULL;
    }

    MethodCacheEntry* get_single_cache() {
      if(cache_size() == 1) {
        return cache_[0].entry();
      }
      return NULL;
    }

    int growth_cache_size(uint32_t class_id) {
      int count = 0;
      for(int i = 0; i < cTrackedICHits; ++i) {
        MethodCacheEntry* current = cache_[i].entry();
        if(current && current->receiver_class_id() != class_id) {
          ++count;
        }
      }
      return count;
    }

    int cache_size() {
      for(int i = 0; i < cTrackedICHits; ++i) {
        if(!cache_[i].entry()) return i;
      }
      return cTrackedICHits;
    }

    MethodCacheEntry* get_cache(int idx, int* hits) {
      MethodCacheEntry* entry = cache_[idx].entry();
      if(entry) {
        *hits = cache_[idx].hits();
      }
      return entry;
    }

    void set_cache(MethodCacheEntry* mce) {
      // Make sure we sync here, so the MethodCacheEntry mce is
      // guaranteed completely initialized. Otherwise another thread
      // might see an incompletely initialized MethodCacheEntry.
      int least_used = cTrackedICHits - 1;
      for(int i = 0; i < cTrackedICHits; ++i) {
        MethodCacheEntry* current = cache_[i].entry();
        if(!current || current->receiver_class_id() == mce->receiver_class_id()) {
          cache_[i].assign(mce);
          return;
        }
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

    Class* get_class(int idx, int* hits) {
      MethodCacheEntry* entry = cache_[idx].entry();
      if(entry) {
        *hits = cache_[idx].hits();
        return entry->receiver_class();
      } else {
        return NULL;
      }
    }

  public: // Rubinius Type stuff
    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void set_field(STATE, Object*, size_t, Object*);
      virtual Object* get_field(STATE, Object*, size_t);
      virtual void auto_mark(Object*, ObjectMark&);
      virtual void populate_slot_locations();
    };

  };
}

#endif
