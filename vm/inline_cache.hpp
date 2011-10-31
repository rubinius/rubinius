#ifndef RBX_INLINE_CACHE_HPP
#define RBX_INLINE_CACHE_HPP

#include "dispatch.hpp"
#include "builtin/object.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "lock.hpp"
#include "object_utils.hpp"

#include <vector>
#include <tr1/unordered_map>

// #define TRACK_IC_LOCATION

namespace rubinius {
  class InlineCache;
  struct CallFrame;
  class Arguments;
  class CallUnit;
  class MethodCacheEntry;

  // How many receiver class have been seen to keep track of inside an IC
  const static int cTrackedICHits = 3;

  class InlineCacheHit {
    Class* seen_class_;

  public:

    InlineCacheHit()
      : seen_class_(0)
    {}

    void assign(Class* mod) {
      seen_class_ = mod;
    }

    Class* klass() {
      return seen_class_;
    }

    void set_klass(Class* mod) {
      seen_class_ = mod;
    }

    friend class InlineCache;
    friend class CompiledMethod::Info;
  };

  class InlineCache {
  public:
    Symbol* name;

  private:
    MethodCacheEntry* cache_;
    CallUnit* call_unit_;

    typedef Object* (*CacheExecutor)(STATE, InlineCache*, CallFrame*, Arguments& args);

    CacheExecutor initial_backend_;
    CacheExecutor execute_backend_;

#ifdef TRACK_IC_LOCATION
    int ip_;
    VMMethod* vmm_;
#endif

    int seen_classes_overflow_;
    InlineCacheHit seen_classes_[cTrackedICHits];
    int private_lock_;

  public:

    friend class CompiledMethod::Info;

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

    static Object* disabled_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);
    static Object* disabled_cache_private(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);
    static Object* disabled_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);

    MethodMissingReason fill_public(STATE, Object* self, Symbol* name, Class* klass,
                                    MethodCacheEntry*& mce);
    bool fill_private(STATE, Symbol* name, Module* start, Class* klass,
                      MethodCacheEntry*& mce);
    bool fill_method_missing(STATE, Class* klass, MethodCacheEntry*& mce);

    MethodCacheEntry* update_and_validate(STATE, CallFrame* call_frame, Object* recv);
    MethodCacheEntry* update_and_validate_private(STATE, CallFrame* call_frame, Object* recv);

    InlineCache()
      : name(0)
      , cache_(0)
      , call_unit_(0)
      , initial_backend_(empty_cache)
      , execute_backend_(empty_cache)
      , seen_classes_overflow_(0)
      , private_lock_(0)
    {}

#ifdef TRACK_IC_LOCATION
    void set_location(int ip, VMMethod* vmm) {
      ip_ = ip;
      vmm_ = vmm;
    }

    int ip() {
      return ip_;
    }

    VMMethod* vmmethod() {
      return vmm_;
    }
#else
    void set_location(int ip, VMMethod* vmm) { }
#endif

    void print_location(STATE, std::ostream& stream);
    void print(STATE, std::ostream& stream);

    void set_name(Symbol* sym) {
      name = sym;
    }

    MethodCacheEntry* cache() {
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
      cache_ = 0;
    }

    void update_seen_classes(MethodCacheEntry* mce);

    int seen_classes_overflow() {
      return seen_classes_overflow_;
    }

    int classes_seen() {
      int seen = 0;
      for(int i = 0; i < cTrackedICHits; i++) {
        if(seen_classes_[i].klass()) seen++;
      }

      return seen;
    }

    Class* tracked_class(int which) {
      return seen_classes_[which].klass();
    }

    Class* find_class_by_id(int64_t id) {
      for(int i = 0; i < cTrackedICHits; i++) {
        Class* cls = seen_classes_[i].klass();
        if(cls && cls->class_id() == id) return cls;
      }

      return 0;
    }

    Class* find_singletonclass(int64_t id) {
      for(int i = 0; i < cTrackedICHits; i++) {
        if(Class* cls = seen_classes_[i].klass()) {
          if(SingletonClass* sc = try_as<SingletonClass>(cls)) {
            if(Class* ref = try_as<Class>(sc->attached_instance())) {
              if(ref->class_id() == id) return cls;
            }
          }
        }
      }

      return 0;
    }


    Class* dominating_class() {
      int seen = classes_seen();
      switch(seen) {
      case 0:
        return NULL;
      case 1:
        return seen_classes_[0].klass();
      /*
       *  These are disabled because they hurt performance.
      case 2: {
        int h1 = seen_classes_[0].hits();
        int h2 = seen_classes_[1].hits();
        if(h2 * 10 < h1) return seen_classes_[0].klass();
        if(h1 * 10 < h2) return seen_classes_[1].klass();
        return NULL;
      }

      case 3: {
        int h1 = seen_classes_[0].hits();
        int h2 = seen_classes_[1].hits();
        int h3 = seen_classes_[2].hits();

        if(h2 * 10 < h1 && h3 * 10 < h1) return seen_classes_[0].klass();
        if(h1 * 10 < h2 && h3 * 10 < h2) return seen_classes_[1].klass();
        if(h1 * 10 < h3 && h2 * 10 < h3) return seen_classes_[2].klass();

        return NULL;
      }
      */
      default:
        return NULL;
      }
    }

  };

  // Registry, used to clear ICs by method name
  class InlineCacheRegistry : public Lockable {
    typedef std::list<InlineCache*> CacheVector;
    typedef std::tr1::unordered_map<native_int, CacheVector> CacheHash;

    CacheHash caches_;

  public:
    void add_cache(STATE, Symbol* sym, InlineCache* cache);
    void remove_cache(STATE, Symbol* sym, InlineCache* cache);
    void clear(STATE, Symbol* sym);

    void print_stats(STATE);
  };
}

#endif
