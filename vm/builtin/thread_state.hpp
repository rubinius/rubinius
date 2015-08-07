#ifndef RBX_BUILTIN_THREAD_STATE_HPP
#define RBX_BUILTIN_THREAD_STATE_HPP

#include "object_utils.hpp"

#include "builtin/object.hpp"
#include "builtin/variable_scope.hpp"

namespace rubinius {

  class ThreadState : public Object {
  public:
    const static object_type type = ThreadStateType;

  private:
    Exception* current_exception_; // slot
    Object* raise_value_; // slot
    Object* throw_dest_; // slot
    Fixnum* raise_reason_; // slot
    VariableScope* destination_scope_; // slot

  public:
    /* accessors */

    attr_accessor(current_exception, Exception);
    attr_accessor(raise_value, Object);
    attr_accessor(throw_dest, Object);
    attr_accessor(raise_reason, Fixnum);
    attr_accessor(destination_scope, VariableScope);

    /* interface */

    static void bootstrap(STATE);
    static void initialize(STATE, ThreadState* obj) {
      obj->current_exception_ = nil<Exception>();
      obj->raise_value_ = nil<Object>();
      obj->throw_dest_ = nil<Object>();
      obj->raise_reason_ = nil<Fixnum>();
      obj->destination_scope_ = nil<VariableScope>();
    }

    static ThreadState* create(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}

#endif
