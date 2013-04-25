#ifndef RBX_INLINE_CACHE_HPP
#define RBX_INLINE_CACHE_HPP

#include "dispatch.hpp"
#include "builtin/object.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledcode.hpp"
#include "lock.hpp"
#include "object_utils.hpp"

#include <tr1/unordered_map>
#include <tr1/unordered_set>

namespace rubinius {
  class InlineCache;
  struct CallFrame;
  class Arguments;
  class CallUnit;

  class InlineCacheEntry : public Object {
  public:
    const static object_type type = InlineCacheEntryType;

  private:
    Module* stored_module_;  // slot
    Class*  receiver_class_; // slot
    Executable* method_;     // slot

    ClassData receiver_;

    MethodMissingReason method_missing_;

  public:
    attr_accessor(stored_module, Module);
    attr_accessor(receiver_class, Class);
    attr_accessor(method, Executable);

    uint64_t receiver_data() {
      return receiver_.raw;
    }

    uint32_t receiver_class_id() {
      return receiver_.f.class_id;
    }

    uint32_t receiver_serial_id() {
      return receiver_.f.serial_id;
    }

    void set_method_missing(MethodMissingReason reason) {
      method_missing_ = reason;
    }

    MethodMissingReason method_missing() {
      return method_missing_;
    }

  public:
    static InlineCacheEntry* create(STATE, Class* klass, Module* mod,
                                    Executable* method, MethodMissingReason method_missing);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo);
    };
  };

  // How many receiver class have been seen to keep track of inside an IC
  const static int cTrackedICHits = 3;

  class InlineCacheHit {
    InlineCacheEntry* entry_;
    int hits_;

  public:

    InlineCacheHit()
      : entry_(NULL)
      , hits_(0)
    {}

    void assign(InlineCacheEntry* entry) {
      hits_ = 0;
      atomic::write(&entry_, entry);
    }

    void update(InlineCacheEntry* entry) {
      atomic::write(&entry_, entry);
    }

    void hit() {
      ++hits_;
    }

    int hits() {
      return hits_;
    }

    InlineCacheEntry* entry() {
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

    static Object* check_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);

    static Object* check_cache_custom(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_poly(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);

    MethodMissingReason fill(STATE, Object* self, Class* klass, Symbol* name, Module* start,
                             Symbol* vis, InlineCacheEntry*& ice);

    bool fill_method_missing(STATE, Object* self, Class* klass, MethodMissingReason reason, InlineCacheEntry*& ice);

    InlineCacheEntry* update_and_validate(STATE, CallFrame* call_frame, Object* recv, Symbol* vis);

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

    bool is_super_p() {
      return initial_backend_ == empty_cache_super;
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

    InlineCacheEntry* get_cache(Class* const recv_class) {
      register uint64_t recv_data = recv_class->data_id();
      for(int i = 0; i < cTrackedICHits; ++i) {
        InlineCacheEntry* ice = cache_[i].entry();
        if(likely(ice && ice->receiver_data() == recv_data)) return ice;
      }
      return NULL;
    }

    InlineCacheHit* get_inline_cache(Class* const recv_class, InlineCacheEntry*& ice) {
      register uint64_t recv_data = recv_class->data_id();
      for(int i = 0; i < cTrackedICHits; ++i) {
        ice = cache_[i].entry();
        if(likely(ice && ice->receiver_data() == recv_data)) return &cache_[i];
      }
      return NULL;
    }

    InlineCacheEntry* get_single_cache() {
      if(cache_size() == 1) {
        return cache_[0].entry();
      }
      return NULL;
    }

    int growth_cache_size(uint32_t class_id) {
      int count = 0;
      for(int i = 0; i < cTrackedICHits; ++i) {
        InlineCacheEntry* current = cache_[i].entry();
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

    InlineCacheEntry* get_cache(int idx, int* hits) {
      InlineCacheEntry* entry = cache_[idx].entry();
      if(entry) {
        *hits = cache_[idx].hits();
      }
      return entry;
    }

    void set_cache(InlineCacheEntry* ice) {
      // Make sure we sync here, so the InlineCacheEntry ice is
      // guaranteed completely initialized. Otherwise another thread
      // might see an incompletely initialized InlineCacheEntry.
      int least_used = cTrackedICHits - 1;
      for(int i = 0; i < cTrackedICHits; ++i) {
        InlineCacheEntry* current = cache_[i].entry();
        if(!current || current->receiver_class_id() == ice->receiver_class_id()) {
          cache_[i].assign(ice);
          return;
        }
        if(cache_[i].hits() < cache_[least_used].hits()) {
          least_used = i;
        }
      }

      seen_classes_overflow_++;
      cache_[least_used].assign(ice);
    }

    int seen_classes_overflow() {
      return seen_classes_overflow_;
    }

    int classes_seen() {
      return cache_size();
    }

    Class* get_class(int idx, int* hits) {
      InlineCacheEntry* entry = cache_[idx].entry();
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
