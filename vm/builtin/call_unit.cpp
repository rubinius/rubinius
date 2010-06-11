#include "builtin/call_unit.hpp"
#include "builtin/system.hpp"
#include "builtin/class.hpp"
#include "builtin/location.hpp"

#include "dispatch.hpp"
#include "call_frame.hpp"
#include "arguments.hpp"

#include "object_utils.hpp"

namespace rubinius {
  CallUnit* CallUnit::create_constant_value(STATE, Object* self, Object* val) {
    CallUnit* pe = state->new_object<CallUnit>(as<Class>(self));
    pe->kind_ = CallUnit::eConstantValue;
    pe->value(state, val);

    pe->execute = constant_value_executor;
    return pe;
  }

  CallUnit* CallUnit::create_for_method(STATE, Object* self, Module* mod, Executable* exec, Symbol* name) {
    CallUnit* pe = state->new_object<CallUnit>(as<Class>(self));
    pe->kind_ = CallUnit::eForMethod;
    pe->module(state, mod);
    pe->executable(state, exec);
    pe->name(state, name);

    pe->execute = method_executor;
    return pe;
  }

  CallUnit* CallUnit::create_test(STATE, Object* self,
                      CallUnit* cond, CallUnit* c_then, CallUnit* c_else)
  {
    CallUnit* pe = state->new_object<CallUnit>(as<Class>(self));
    pe->kind_ = CallUnit::eTest;
    pe->test_condition(state, cond);
    pe->test_then(state, c_then);
    pe->test_else(state, c_else);

    pe->execute = test_executor;
    return pe;
  }

  CallUnit* CallUnit::create_kind_of(STATE, Object* self, Module* mod, Fixnum* which) {
    CallUnit* pe = state->new_object<CallUnit>(as<Class>(self));
    pe->kind_ = CallUnit::eKindOf;
    pe->value(state, mod);
    pe->which_ = which->to_native();

    pe->execute = kind_of_executor;
    return pe;
  }

  Object* CallUnit::constant_value_executor(STATE, CallFrame* call_frame,
                                            CallUnit* unit,
                                            Dispatch& msg,
                                            Arguments& args)
  {
    return unit->value();
  }

  Object* CallUnit::method_executor(STATE, CallFrame* call_frame,
                                            CallUnit* unit,
                                            Dispatch& msg,
                                            Arguments& args)
  {
    msg.method = unit->executable();
    msg.name = unit->name();
    msg.module = unit->module();
    return unit->executable()->execute(state, call_frame, msg, args);
  }

  Object* CallUnit::test_executor(STATE, CallFrame* call_frame,
                                            CallUnit* unit,
                                            Dispatch& msg,
                                            Arguments& args)
  {
    Object* ret = unit->test_condition()->execute(
             state, call_frame, unit->test_condition(), msg, args);
    if(!ret) return ret;
    if(RTEST(ret)) {
      return unit->test_then()->execute(state, call_frame, unit->test_then(), msg, args);
    } else {
      return unit->test_else()->execute(state, call_frame, unit->test_else(), msg, args);
    }
  }

  Object* CallUnit::kind_of_executor(STATE, CallFrame* call_frame,
                                            CallUnit* unit,
                                            Dispatch& msg,
                                            Arguments& args)
  {
    Object* obj;
    if(unit->which_ == -1) {
      obj = args.recv();
    } else if(unit->which_ < (int)args.total()) {
      obj = args.get_argument(unit->which_);
    } else {
      return Qfalse;
    }

    if(Module* mod = try_as<Module>(unit->value())) {
      return obj->kind_of_p(state, mod) ? Qtrue : Qfalse;
    } else {
      return Qfalse;
    }
  }
}
