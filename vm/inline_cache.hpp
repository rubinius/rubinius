#include "dispatch.hpp"
#include "builtin/object.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"

#include <vector>
#include <tr1/unordered_map>

namespace rubinius {
  class InlineCache;
  class CallFrame;
  class Arguments;

  // How many receiver class have been seen to keep track of inside an IC
  const static int cTrackedICHits = 3;

  class InlineCacheHit {
    Module* module_;
    int hits_;

  public:

    InlineCacheHit()
      : module_(0)
      , hits_(0)
    {}

    Module* module() {
      return module_;
    }

    int hits() {
      return hits_;
    }

    friend class InlineCache;
  };

  class InlineCache : public Dispatch {
    Module* klass_;

    typedef Object* (*CacheExecutor)(STATE, InlineCache*, CallFrame*, Arguments& args);

    CacheExecutor initial_backend_;
    CacheExecutor execute_backend_;

    int* hits_;
    int seen_classes_overflow_;
    InlineCacheHit seen_classes_[cTrackedICHits];

  public:

    friend class CompiledMethod::Info;

    static Object* empty_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_private(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_super(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                               Arguments& args);

    static Object* check_cache_mm(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);

    static Object* disabled_cache(STATE, InlineCache* cache, CallFrame* call_frame,
                                  Arguments& args);

    bool fill_public(STATE, Object* self, Symbol* name);
    bool fill_private(STATE, Symbol* name, Module* start);
    bool fill_method_missing(STATE, Module* start);

    void run_wb(STATE, CompiledMethod* exec);

    InlineCache()
      : Dispatch()
      , klass_(0)
      , initial_backend_(empty_cache)
      , execute_backend_(empty_cache)
      , hits_(0)
      , seen_classes_overflow_(0)
    {}

    void set_name(Symbol* sym) {
      name = sym;
    }

    Module* klass() {
      return klass_;
    }

    void set_klass(Module* klass) {
      klass_ = klass;
    }

    void set_is_private() {
      initial_backend_ = empty_cache_private;
      execute_backend_ = empty_cache_private;
    }

    void set_is_super() {
      initial_backend_ = empty_cache_super;
      execute_backend_ = empty_cache_super;
    }

    Object* execute(STATE, CallFrame* call_frame, Arguments& args) {
      return (*execute_backend_)(state, this, call_frame, args);
    }

    Object* initialize(STATE, CallFrame* call_frame, Arguments& args) {
      return (*initial_backend_)(state, this, call_frame, args);
    }

    bool valid_p(STATE, Object* obj) {
      return klass_ == obj->lookup_begin(state);
    }

    void clear() {
      klass_ = 0;
    }

    void update_seen_classes() {
      for(int i = 0; i < cTrackedICHits; i++) {
        Module* mod = seen_classes_[i].module_;
        if(mod == klass_) {
          hits_ = &seen_classes_[i].hits_;
          return;
        } else if(!mod) {
          // An empty space, record it.
          seen_classes_[i].module_ = mod;
          hits_ = &seen_classes_[i].hits_;
          return;
        }
      }

      // Hmmm, what do we do when this is full? Just ignore them?
      // For now, just keep track of how many times we overflow.
      seen_classes_overflow_++;
    }

    int seen_classes_overflow() {
      return seen_classes_overflow_;
    }

    int hits() {
      return *hits_;
    }

  };

  // Registry, used to clear ICs by method name
  class InlineCacheRegistry {
    typedef std::vector<InlineCache*> CacheVector;
    typedef std::tr1::unordered_map<native_int, CacheVector> CacheHash;

    CacheHash caches_;

  public:
    void add_cache(Symbol* sym, InlineCache* cache);
    void clear(Symbol* sym);
  };
}
