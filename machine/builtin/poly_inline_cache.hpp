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

    attr_accessor(stored_module, Module);
    attr_accessor(receiver_class, Class);
    attr_accessor(method, Executable);

  private:
    attr_field(receiver_data, ClassData);
    attr_field(method_missing, MethodMissingReason);
    attr_field(hits, int);

  public:
    uint64_t receiver_data_raw() const {
      return receiver_data().raw;
    }

    uint32_t receiver_class_id() const {
      return receiver_data().f.class_id;
    }

    uint32_t receiver_serial_id() const {
      return receiver_data().f.serial_id;
    }

    void hit() {
      ++_hits_;
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

    attr_field(seen_classes_overflow, int);

  public:
    static void bootstrap(STATE);
    static PolyInlineCache* create(STATE, MonoInlineCache* mono);

    static CacheExecuteFunc check_cache;
    static CacheExecuteFunc check_cache_mm;

    static CacheUpdateFunc inline_cache_updater;

    void print(STATE, std::ostream& stream);

    void clear() {
      for(int i = 0; i < cTrackedICHits; ++i) {
        entries_[i] = NULL;
      }
    }

    InlineCacheEntry* get_entry(Class* const recv_class) const {
      uint64_t recv_data = recv_class->data_raw();
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
      state->memory()->write_barrier(this, ice);
      if(ice->method_missing() != eNone) {
        executor(check_cache_mm);
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

      _seen_classes_overflow_++;
      atomic::write(&entries_[least_used], ice);
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
      virtual void mark(Object* t, memory::ObjectMark& mark);
    };

  };
}

#endif
