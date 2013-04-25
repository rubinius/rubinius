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

    CacheExecutor execute_backend_;

    Symbol* name_; // slot
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
      execute_backend_ = empty_cache_private;
    }

    void set_is_super() {
      execute_backend_ = empty_cache_super;
    }

    void set_is_vcall() {
      execute_backend_ = empty_cache_vcall;
    }

    void set_call_custom() {
      execute_backend_ = empty_cache_custom;
    }

    void update_call_site(STATE, CallSite* other) {
      if(this != other) {
        if(CompiledCode* ccode = try_as<CompiledCode>(executable_)) {
          ccode->machine_code()->store_call_site(state, ccode, ip_, other);
        }
      }
    }

    Object* execute(STATE, CallFrame* call_frame, Arguments& args) {
      return (*execute_backend_)(state, this, call_frame, args);
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

