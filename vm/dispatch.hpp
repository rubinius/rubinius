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

    Dispatch(Symbol* name)
      : name(name)
      , module(0)
      , method(0)
      , method_missing(false)
    {}

    Object* send(STATE, CallFrame* call_frame, Arguments& args,
                 MethodMissingReason reason = eNormal);
    Object* send(STATE, CallFrame* call_frame, LookupData& lookup, Arguments& args,
                 MethodMissingReason reason = eNormal);
  };
}

#endif
