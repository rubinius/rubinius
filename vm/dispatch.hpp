#ifndef RBX_DISPATCH_HPP
#define RBX_DISPATCH_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class Symbol;
  class Module;
  class Executable;
  struct CallFrame;
  class Arguments;
  struct LookupData;

  class Dispatch {
  public:
    Symbol* name;
    Module* module;
    Executable* method;
    bool method_missing;

    Dispatch()
      : name(0)
      , module(0)
      , method(0)
      , method_missing(false)
    {}

    Dispatch(Symbol* name)
      : name(name)
      , module(reinterpret_cast<Module*>(Qnil))
      , method(reinterpret_cast<Executable*>(Qnil))
      , method_missing(false)
    {}

    Dispatch(Symbol* name, Module* mod, Executable* meth)
      : name(name)
      , module(mod)
      , method(meth)
      , method_missing(false)
    {}

    Object* send(STATE, CallFrame* call_frame, Arguments& args,
                 MethodMissingReason reason = eNormal);
    Object* send(STATE, CallFrame* call_frame, LookupData& lookup, Arguments& args,
                 MethodMissingReason reason = eNormal);
  };
}

#endif
