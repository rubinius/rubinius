#ifndef RBX_CLASS_UNWIND_STATE_HPP
#define RBX_CLASS_UNWIND_STATE_HPP

#include "object_utils.hpp"
#include "raise_reason.hpp"

#include "class/object.hpp"
#include "class/exception.hpp"
#include "class/variable_scope.hpp"

namespace rubinius {

  class UnwindState : public Object {
  public:
    const static object_type type = UnwindStateType;

    attr_accessor(current_exception, Exception);
    attr_accessor(raise_value, Object);
    attr_accessor(throw_dest, Object);
    attr_accessor(destination_scope, VariableScope);

    attr_field(raise_reason, RaiseReason);

    static void bootstrap(STATE);
    static void initialize(STATE, UnwindState* obj) {
      obj->current_exception(nil<Exception>());
      obj->raise_value(nil<Object>());
      obj->throw_dest(nil<Object>());
      obj->destination_scope(nil<VariableScope>());
      obj->raise_reason(cNone);
    }

    static UnwindState* create(STATE);

    UnwindState* copy(STATE);

    void clear_break();
    void clear_return();
    void clear_raise();
    void clear();

    void set_state(UnwindState* unwind_state);

    void raise_exception(Exception* exc);
    void raise_return(Object* value, VariableScope* dest);
    void raise_break(Object* value, VariableScope* dest);
    void raise_exit(Object* code);
    void raise_throw(Object* dest, Object* value);
    void raise_thread_kill();
    void raise_fiber_cancel();

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

}

#endif
