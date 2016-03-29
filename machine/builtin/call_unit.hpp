#ifndef RBX_BUILTIN_CALL_UNIT_HPP
#define RBX_BUILTIN_CALL_UNIT_HPP

#include "object_utils.hpp"

#include "builtin/executable.hpp"
#include "builtin/module.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"

namespace rubinius {
  class CallUnit : public Object {
  public:
    const static object_type type = CallUnitType;

    enum Kind {
      eUnset,
      eConstantValue,
      eForMethod,
      eTest,
      eKindOf
    };

    typedef Object* (*Execute)(State*, CallUnit* unit,
                               Executable* exec, Module* mod, Arguments& args);

    attr_accessor(value, Object);
    attr_accessor(module, Module);
    attr_accessor(executable, Executable);
    attr_accessor(name, Symbol);

    attr_accessor(test_condition, CallUnit);
    attr_accessor(test_then, CallUnit);
    attr_accessor(test_else, CallUnit);

  private:
    attr_field(kind, Kind);
    attr_field(which, int);

  public:
    Execute execute;

    static void initialize(STATE, CallUnit* obj) {
      obj-> kind(eUnset);
      obj->value(nil<Object>());
      obj->module(nil<Module>());
      obj->executable(nil<Executable>());
      obj->name(nil<Symbol>());
      obj->test_condition(nil<CallUnit>());
      obj->test_then(nil<CallUnit>());
      obj->test_else(nil<CallUnit>());
    }

    // Rubinius.primitive :callunit_constant_value
    static CallUnit* create_constant_value(STATE, Object* self, Object* val);

    // Rubinius.primitive :callunit_for_method_parts
    static CallUnit* create_for_method(STATE, Object* self, Module* mod, Executable* exec, Symbol* name);

    // Rubinius.primitive :callunit_test
    static CallUnit* create_test(STATE, Object* self, CallUnit* cond, CallUnit* c_then, CallUnit* c_else);

    // Rubinius.primitive :callunit_kind_of
    static CallUnit* create_kind_of(STATE, Object* self, Module* mod, Fixnum* which);

    static Object* constant_value_executor(STATE,
                                            CallUnit* unit,
                                            Executable* exec, Module* mod,
                                            Arguments& args);

    static Object* method_executor(STATE,
                                            CallUnit* unit,
                                            Executable* exec, Module* mod,
                                            Arguments& args);

    static Object* test_executor(STATE,
                                            CallUnit* unit,
                                            Executable* exec, Module* mod,
                                            Arguments& args);

    static Object* kind_of_executor(STATE,
                                            CallUnit* unit,
                                            Executable* exec, Module* mod,
                                            Arguments& args);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
