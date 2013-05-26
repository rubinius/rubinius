#ifndef RBX_DISPATCH_HPP
#define RBX_DISPATCH_HPP

#include "vm.hpp"

namespace rubinius {
  class Symbol;
  class Module;
  class Object;
  class Executable;
  struct CallFrame;
  class Arguments;
  struct LookupData;

  class Dispatch {
  public:
    Symbol* name;
    Module* module;
    Executable* method;
    Symbol* visibility;
    MethodMissingReason method_missing;

    Dispatch(Symbol* name)
      : name(name)
      , module(0)
      , method(0)
      , visibility(0)
      , method_missing(eNone)
    {}

    Object* send(STATE, CallFrame* call_frame, Arguments& args,
                 MethodMissingReason reason = eNormal);
    Object* send(STATE, CallFrame* call_frame, LookupData& lookup, Arguments& args,
                 MethodMissingReason reason = eNormal);
    bool resolve(STATE, Symbol* name, LookupData& lookup);
  };
}

#endif
