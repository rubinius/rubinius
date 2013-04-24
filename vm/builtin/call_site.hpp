#ifndef RBX_BUILTIN_CALL_SITE_HPP
#define RBX_BUILTIN_CALL_SITE_HPP

#include "machine_code.hpp"
#include "builtin/object.hpp"
#include "builtin/exception.hpp"
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

