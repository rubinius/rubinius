#ifndef RBX_BUILTIN_CALL_SITE_HPP
#define RBX_BUILTIN_CALL_SITE_HPP

#include "machine_code.hpp"
#include "object_utils.hpp"

#include "builtin/executable.hpp"
#include "builtin/object.hpp"

namespace rubinius {

  class Dispatch;

  typedef Object* (CacheExecuteFunc)(STATE, CallSite*, Arguments&);
  typedef Object* (FallbackExecuteFunc)(STATE, CallSite*, Arguments&);
  typedef void (CacheUpdateFunc)(STATE, CallSite*, Class*, Dispatch&);

  typedef CacheExecuteFunc* CacheExecutor;
  typedef FallbackExecuteFunc* FallbackExecutor;
  typedef CacheUpdateFunc* CacheUpdater;

  class CallSiteInformation {
  public:
    CallSiteInformation(Executable* e, int i)
      : executable(e)
      , ip(i)
    {}

    Executable* executable;
    int ip;
  };

  class CallSite : public Object {
  public:
    const static object_type type = CallSiteType;

    attr_accessor(name, Symbol);

    attr_field(executor, CacheExecutor);
    attr_field(fallback, FallbackExecutor);
    attr_field(updater, CacheUpdater);

    attr_accessor(executable, Executable);

  private:
    attr_field(ip, int);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, CallSite* obj) {
      obj->name(nil<Symbol>());
      obj->executor(empty_cache);
      obj->fallback(empty_cache);
      obj->updater(empty_cache_updater);
      obj->executable(nil<Executable>());
      obj->ip(0);
    }

    // Rubinius.primitive+ :call_site_ip
    Integer* ip_prim(STATE);

    static CallSite* empty(STATE, Symbol* name, Executable* executable, int ip);

    static CacheExecuteFunc empty_cache;
    static CacheExecuteFunc empty_cache_private;
    static CacheExecuteFunc empty_cache_vcall;
    static CacheExecuteFunc empty_cache_super;

    static CacheUpdateFunc empty_cache_updater;

    bool regular_call() const {
      return type_id() == MonoInlineCacheType || type_id() == PolyInlineCacheType;
    }

    bool update_and_validate(STATE, Object* recv, Symbol* vis, int serial);

    void set_is_private() {
      executor(empty_cache_private);
      fallback(empty_cache_private);
    }

    void set_is_super() {
      executor(empty_cache_super);
      fallback(empty_cache_super);
    }

    void set_is_vcall() {
      executor(empty_cache_vcall);
      fallback(empty_cache_vcall);
    }

    void set_executor(CacheExecutor exec) {
      executor(exec);
      fallback(exec);
    }

    void update_call_site(STATE, CallSite* other) {
      if(this != other) {
        if(CompiledCode* ccode = try_as<CompiledCode>(executable())) {
          ccode->machine_code()->store_call_site(state, ccode, ip(), other);
        }
      }
    }

    static bool lookup_method_missing(STATE, Arguments& args,
        Dispatch& dis, Object* self, Module* begin);

    Object* execute(STATE, Arguments& args) {
      return _executor_(state, this, args);
    }

    Object* fallback(STATE, Arguments& args) {
      return _fallback_(state, this, args);
    }

    void update(STATE, Class* klass, Dispatch& dispatch) {
      _updater_(state, this, klass, dispatch);
    }

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(Object* obj, memory::ObjectMark& mark);
      virtual void auto_mark(Object* obj, memory::ObjectMark& mark);
      virtual void set_field(STATE, Object* target, size_t index, Object* val);
      virtual Object* get_field(STATE, Object* target, size_t index);
      virtual void populate_slot_locations();
    };
  };

}

#endif

