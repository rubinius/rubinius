#ifndef RBX_INLINE_CACHE_HPP
#define RBX_INLINE_CACHE_HPP

#include "builtin/class.hpp"
#include "builtin/call_site.hpp"

namespace rubinius {
  struct CallFrame;
  class Arguments;
  class Dispatch;
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
    int hits_;

  public:
    attr_accessor(stored_module, Module);
    attr_accessor(receiver_class, Class);
    attr_accessor(method, Executable);

    ClassData receiver_data() const {
      return receiver_;
    }

    uint64_t receiver_data_raw() const {
      return receiver_.raw;
    }

    uint32_t receiver_class_id() const {
      return receiver_.f.class_id;
    }

    uint32_t receiver_serial_id() const {
      return receiver_.f.serial_id;
    }

    void set_method_missing(MethodMissingReason reason) {
      method_missing_ = reason;
    }

    MethodMissingReason method_missing() const {
      return method_missing_;
    }

    int hits() const {
      return hits_;
    }

    void hit() {
      ++hits_;
    }

    // Rubinius.primitive+ :inline_cache_entry_hits
    Integer* hits_prim(STATE);

    // Rubinius.primitive+ :inline_cache_entry_method_missing
    Symbol* method_missing_prim(STATE);

  public:
    static InlineCacheEntry* create(STATE, ClassData data, Class* klass, Dispatch& dis, int hits);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo);
    };
  };

  // How many receiver class have been seen to keep track of inside an IC
  const static int cTrackedICHits = 3;

  class PolyInlineCache : public CallSite {
  public:
    const static object_type type = PolyInlineCacheType;

  private:
    InlineCacheEntry* entries_[cTrackedICHits];

    int seen_classes_overflow_;

  public:
    static void init(STATE);
    static PolyInlineCache* create(STATE, MonoInlineCache* mono);

    static Object* check_cache(STATE, CallSite* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_mm(STATE, CallSite* cache, CallFrame* call_frame,
                                  Arguments& args);

    static void inline_cache_updater(STATE, CallSite* call_site, Class* klass, Dispatch& dispatch);

    void print(STATE, std::ostream& stream);

    void clear() {
      for(int i = 0; i < cTrackedICHits; ++i) {
        entries_[i] = NULL;
      }
    }

    InlineCacheEntry* get_entry(Class* const recv_class) const {
      register uint64_t recv_data = recv_class->data_raw();
      for(int i = 0; i < cTrackedICHits; ++i) {
        InlineCacheEntry* ice = entries_[i];
        if(likely(ice && ice->receiver_data_raw() == recv_data)) return ice;
      }
      return NULL;
    }

    int cache_size() const {
      for(int i = 0; i < cTrackedICHits; ++i) {
        if(!entries_[i]) return i;
      }
      return cTrackedICHits;
    }

    InlineCacheEntry* get_cache(int idx) const {
      return entries_[idx];
    }

    void set_cache(STATE, InlineCacheEntry* ice) {
      write_barrier(state, ice);
      if(ice->method_missing() != eNone) {
        executor_ = check_cache_mm;
      }
      // Make sure we sync here, so the InlineCacheEntry ice is
      // guaranteed completely initialized. Otherwise another thread
      // might see an incompletely initialized InlineCacheEntry.
      int least_used = 0;
      for(int i = 0; i < cTrackedICHits; ++i) {
        InlineCacheEntry* current = entries_[i];
        if(!current || current->receiver_class_id() == ice->receiver_class_id()) {
          atomic::write(&entries_[i], ice);
          return;
        }
        if(current && current->hits() < entries_[least_used]->hits()) {
          least_used = i;
        }
      }

      seen_classes_overflow_++;
      atomic::write(&entries_[least_used], ice);
    }

    int seen_classes_overflow() const {
      return seen_classes_overflow_;
    }

    int classes_seen() const {
      return cache_size();
    }

    // Rubinius.primitive+ :poly_inline_cache_entries
    Tuple* entries_prim(STATE);

    // Rubinius.primitive+ :poly_inline_cache_overflows
    Integer* overflows(STATE);

  public: // Rubinius Type stuff
    class Info : public CallSite::Info {
    public:
      BASIC_TYPEINFO(CallSite::Info)
      virtual void mark(Object* t, ObjectMark& mark);
    };

  };
}

#endif
