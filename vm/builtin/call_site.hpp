#ifndef RBX_BUILTIN_CALL_SITE_HPP
#define RBX_BUILTIN_CALL_SITE_HPP

#include "machine_code.hpp"
#include "builtin/object.hpp"
#include "builtin/exception.hpp"
#include "machine_code.hpp"
#include "object_utils.hpp"
#include "type_info.hpp"

namespace rubinius {

  typedef Object* (*CacheExecutor)(STATE, CallSite*, CallFrame*, Arguments& args);

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
    CacheExecutor executor_;
    CacheExecutor fallback_;

    Executable* executable_; // slot
    int ip_;

  public:
    attr_accessor(name, Symbol);
    attr_accessor(executable, Executable);

    static void init(STATE);

    int ip() {
      return ip_;
    }

    static CallSite* empty(STATE, Symbol* name, Executable* executable, int ip);

    static Object* empty_cache(STATE, CallSite* call_site, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_private(STATE, CallSite* call_site, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_vcall(STATE, CallSite* call_site, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_super(STATE, CallSite* call_site, CallFrame* call_frame,
                               Arguments& args);

    static Object* empty_cache_custom(STATE, CallSite* call_site, CallFrame* call_frame,
                                          Arguments& args);

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

    void set_executor(CacheExecutor executor) {
      executor_ = executor;
      fallback_ = executor;
    }

    void update_call_site(STATE, CallSite* other) {
      if(this != other) {
        if(CompiledCode* ccode = try_as<CompiledCode>(executable_)) {
          ccode->machine_code()->store_call_site(state, ccode, ip_, other);
        }
      }
    }

    Object* execute(STATE, CallFrame* call_frame, Arguments& args) {
      return (*executor_)(state, this, call_frame, args);
    }

    Object* fallback(STATE, CallFrame* call_frame, Arguments& args) {
      return (*fallback_)(state, this, call_frame, args);
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void mark(Object* t, ObjectMark& mark);
    };

    friend class Info;
  };

}

#endif

