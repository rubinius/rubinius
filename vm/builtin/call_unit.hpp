#ifndef RBX_BUILTIN_CALL_UNIT_HPP
#define RBX_BUILTIN_CALL_UNIT_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class CallUnit : public Object {
  public:
    const static object_type type = CallUnitType;

    enum Kind {
      eConstantValue,
      eForMethod,
      eTest,
      eKindOf
    };

    typedef Object* (*Execute)(VM*, CallFrame*, CallUnit* unit,
                               Dispatch& msg, Arguments& args);

  private:
    Kind kind_;

    Object* value_; // slot
    Module* module_; // slot
    Executable* executable_; // slot
    Symbol* name_; // slot

    CallUnit* test_condition_; // slot
    CallUnit* test_then_; // slot
    CallUnit* test_else_; // slot

    int which_;

  public:
    Execute execute;

  public:
    attr_accessor(value, Object);
    attr_accessor(module, Module);
    attr_accessor(executable, Executable);
    attr_accessor(name, Symbol);

    attr_accessor(test_condition, CallUnit);
    attr_accessor(test_then, CallUnit);
    attr_accessor(test_else, CallUnit);

    // Ruby.primitive :callunit_constant_value
    static CallUnit* create_constant_value(STATE, Object* self, Object* val);

    // Ruby.primitive :callunit_for_method_parts
    static CallUnit* create_for_method(STATE, Object* self, Module* mod, Executable* exec, Symbol* name);

    // Ruby.primitive :callunit_test
    static CallUnit* create_test(STATE, Object* self, CallUnit* cond, CallUnit* c_then, CallUnit* c_else);

    // Ruby.primitive :callunit_kind_of
    static CallUnit* create_kind_of(STATE, Object* self, Module* mod, Fixnum* which);

    static Object* constant_value_executor(STATE, CallFrame* call_frame,
                                            CallUnit* unit,
                                            Dispatch& msg,
                                            Arguments& args);

    static Object* method_executor(STATE, CallFrame* call_frame,
                                            CallUnit* unit,
                                            Dispatch& msg,
                                            Arguments& args);

    static Object* test_executor(STATE, CallFrame* call_frame,
                                            CallUnit* unit,
                                            Dispatch& msg,
                                            Arguments& args);

    static Object* kind_of_executor(STATE, CallFrame* call_frame,
                                            CallUnit* unit,
                                            Dispatch& msg,
                                            Arguments& args);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
