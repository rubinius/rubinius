#ifndef RBX_BUILTIN_CALL_SITE_HPP
#define RBX_BUILTIN_CALL_SITE_HPP

#include "builtin/object.hpp"
#include "machine_code.hpp"
#include "object_utils.hpp"

namespace rubinius {

  class Dispatch;

  typedef Object* (CacheExecuteFunc)(STATE, CallSite*, CallFrame*, Arguments&);
  typedef Object* (FallbackExecuteFunc)(STATE, CallSite*, CallFrame*, Arguments&);
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

    Symbol* name_; // slot
    CacheExecutor    executor_;
    FallbackExecutor fallback_;
    CacheUpdater     updater_;

    Executable* executable_; // slot
    int ip_;

  public:
    attr_accessor(name, Symbol);
    attr_accessor(executable, Executable);

    static void init(STATE);

    int ip() const {
      return ip_;
    }

    // Rubinius.primitive+ :call_site_ip
    Integer* ip_prim(STATE);

    static CallSite* empty(STATE, Symbol* name, Executable* executable, int ip);

    static CacheExecuteFunc empty_cache;
    static CacheExecuteFunc empty_cache_private;
    static CacheExecuteFunc empty_cache_vcall;
    static CacheExecuteFunc empty_cache_super;
    static CacheExecuteFunc empty_cache_custom;

    static CacheUpdateFunc empty_cache_updater;

    bool regular_call() const {
      return type_id() == MonoInlineCacheType || type_id() == PolyInlineCacheType;
    }

    bool update_and_validate(STATE, CallFrame* call_frame, Object* recv, Symbol* vis, int serial);

    void set_is_private() {
      executor_ = empty_cache_private;
      fallback_ = empty_cache_private;
    }

    void set_is_super() {
      executor_ = empty_cache_super;
      fallback_ = empty_cache_super;
    }

    void set_is_vcall() {
      executor_ = empty_cache_vcall;
      fallback_ = empty_cache_vcall;
    }

    void set_call_custom() {
      executor_ = empty_cache_custom;
      fallback_ = empty_cache_custom;
    }

    void set_executor(CacheExecutor exec) {
      executor_ = exec;
      fallback_ = exec;
    }

    void update_call_site(STATE, CallSite* other) {
      if(this != other) {
        if(CompiledCode* ccode = try_as<CompiledCode>(executable_)) {
          ccode->machine_code()->store_call_site(state, ccode, ip_, other);
        }
      }
    }

    static bool lookup_method_missing(STATE, CallFrame* call_frame, Arguments& args, Dispatch& dis, Object* self, Module* begin);

    Object* execute(STATE, CallFrame* call_frame, Arguments& args) {
      return (*executor_)(state, this, call_frame, args);
    }

    Object* fallback(STATE, CallFrame* call_frame, Arguments& args) {
      return (*fallback_)(state, this, call_frame, args);
    }

    void update(STATE, Class* klass, Dispatch& dispatch) {
      (*updater_)(state, this, klass, dispatch);
    }

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(Object* obj, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark);
      virtual void set_field(STATE, Object* target, size_t index, Object* val);
      virtual Object* get_field(STATE, Object* target, size_t index);
      virtual void populate_slot_locations();
    };
  };

}

#endif

