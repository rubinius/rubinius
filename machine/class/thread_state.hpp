#ifndef RBX_BUILTIN_THREAD_STATE_HPP
#define RBX_BUILTIN_THREAD_STATE_HPP

#include "object_utils.hpp"

#include "class/object.hpp"
#include "class/variable_scope.hpp"

namespace rubinius {

  class ThreadState : public Object {
  public:
    const static object_type type = ThreadStateType;

    attr_accessor(current_exception, Exception);
    attr_accessor(raise_value, Object);
    attr_accessor(throw_dest, Object);
    attr_accessor(raise_reason, Fixnum);
    attr_accessor(destination_scope, VariableScope);

    static void bootstrap(STATE);
    static void initialize(STATE, ThreadState* obj) {
      obj->current_exception(nil<Exception>());
      obj->raise_value(nil<Object>());
      obj->throw_dest(nil<Object>());
      obj->raise_reason(nil<Fixnum>());
      obj->destination_scope(nil<VariableScope>());
    }

    static ThreadState* create(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}

#endif
