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
  class CompiledMethod::Info;

  class InlineCache : public Dispatch {
    Module* klass_;

    typedef Object* (*CacheExecutor)(STATE, InlineCache*, CallFrame*, Arguments& args);

    CacheExecutor initial_backend_;
    CacheExecutor execute_backend_;

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

    bool fill_public(STATE, Object* self, Symbol* name);
    bool fill_private(STATE, Symbol* name, Module* start);

    void run_wb(STATE, CompiledMethod* exec);

    InlineCache()
      : Dispatch()
      , klass_(0)
      , initial_backend_(empty_cache)
      , execute_backend_(empty_cache)
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
