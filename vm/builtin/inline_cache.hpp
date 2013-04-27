#ifndef RBX_INLINE_CACHE_HPP
#define RBX_INLINE_CACHE_HPP

#include "dispatch.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/call_site.hpp"
#include "builtin/class.hpp"
#include "lock.hpp"
#include "object_utils.hpp"

namespace rubinius {
  class InlineCache;
  struct CallFrame;
  class Arguments;
  class Module;
  class MonoInlineCache;

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

    ClassData receiver_data() {
      return receiver_;
    }

    uint64_t receiver_data_raw() {
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
    static InlineCacheEntry* create(STATE, ClassData data, Class* klass, Dispatch& dis);

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

  class InlineCache : public CallSite {
  public:
    const static object_type type = InlineCacheType;

  private:
    InlineCacheHit cache_[cTrackedICHits];

    int seen_classes_overflow_;

  public:
    static void init(STATE);
    static InlineCache* create(STATE, MonoInlineCache* mono);

    static Object* check_cache(STATE, CallSite* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_mm(STATE, CallSite* cache, CallFrame* call_frame,
                                  Arguments& args);

    static void inline_cache_updater(STATE, CallSite* call_site, Class* klass, FallbackExecutor fallback, Dispatch& dispatch);

    void print(STATE, std::ostream& stream);

    InlineCacheHit* caches() {
      return cache_;
    }

    void clear() {
      for(int i = 0; i < cTrackedICHits; ++i) {
        cache_[i].assign(NULL);
      }
    }

    InlineCacheEntry* get_cache(Class* const recv_class) {
      register uint64_t recv_data = recv_class->data_raw();
      for(int i = 0; i < cTrackedICHits; ++i) {
        InlineCacheEntry* ice = cache_[i].entry();
        if(likely(ice && ice->receiver_data_raw() == recv_data)) return ice;
      }
      return NULL;
    }

    InlineCacheHit* get_inline_cache(Class* const recv_class, InlineCacheEntry*& ice) {
      register uint64_t recv_data = recv_class->data_raw();
      for(int i = 0; i < cTrackedICHits; ++i) {
        ice = cache_[i].entry();
        if(likely(ice && ice->receiver_data_raw() == recv_data)) return &cache_[i];
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

    void set_cache(STATE, InlineCacheEntry* ice) {
      write_barrier(state, ice);
      if(ice->method_missing() != eNone) {
        executor_ = check_cache_mm;
      }
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
    class Info : public CallSite::Info {
    public:
      BASIC_TYPEINFO(CallSite::Info)
      virtual void mark(Object* t, ObjectMark& mark);
    };

  };
}

#endif
