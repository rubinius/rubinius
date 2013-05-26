#ifndef RBX_BUILTIN_THREAD_STATE_HPP
#define RBX_BUILTIN_THREAD_STATE_HPP

#include "builtin/object.hpp"

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

    static void init(STATE);

    static ThreadState* create(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}

#endif
