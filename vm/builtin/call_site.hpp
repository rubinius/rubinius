#ifndef RBX_BUILTIN_CALL_SITE_HPP
#define RBX_BUILTIN_CALL_SITE_HPP

#include "builtin/object.hpp"
#include "builtin/exception.hpp"
#include "type_info.hpp"

namespace rubinius {

  typedef Object* (*CacheExecutor)(STATE, CallSite*, CallFrame*, Arguments& args);

  class CallSite : public Object {
  public:
    const static object_type type = CallSiteType;

    Symbol* name_; // slot

    CacheExecutor execute_backend_;

    CallSite** location_;

  public:
    attr_accessor(name, Symbol);

    static void init(STATE);

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

