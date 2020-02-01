#ifndef RBX_DISPATCH_HPP
#define RBX_DISPATCH_HPP

#include "thread_state.hpp"

namespace rubinius {
  class Arguments;
  class Executable;
  class Module;
  class Object;
  class Prediction;
  class Symbol;
  struct LookupData;

  class Dispatch {
  public:
    Symbol* name;
    Module* module;
    Executable* method;
    Symbol* visibility;
    Prediction* prediction;
    MethodMissingReason method_missing;

    Dispatch(Symbol* name)
      : name(name)
      , module(0)
      , method(0)
      , visibility(0)
      , prediction(0)
      , method_missing(eNone)
    {}

    Object* send(STATE, Arguments& args, MethodMissingReason reason = eNormal);
    Object* send(STATE, LookupData& lookup, Arguments& args,
                 MethodMissingReason reason = eNormal);
    bool resolve(STATE, Symbol* name, LookupData& lookup);
  };
}

#endif
