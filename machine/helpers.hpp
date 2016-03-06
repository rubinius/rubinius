#ifndef RBX_HELPERS_HPP
#define RBX_HELPERS_HPP

#include "vm.hpp"
#include "state.hpp"

namespace rubinius {
  struct CallFrame;
  class Module;
  class Class;
  class Symbol;
  class CompiledCode;
  class State;
  class Object;
  class Tuple;

  namespace Helpers {
    Object* const_get_under(State*, Module* under, Symbol* name, ConstantMissingReason* reason, Object* filter = NULL, bool replace_autoload = false);
    Object* const_get(State*, Symbol* name, ConstantMissingReason* reason, Object* filter = NULL, bool replace_autoload = false);
    Object* const_missing_under(State*, Module* under, Symbol* sym);
    Object* const_missing(State*, Symbol* sym);
    void const_set(State*, Symbol* name, Object* val);
    void const_set(State*, Module* mod, Symbol* name, Object* val);

    Class*    open_class(State*, Object* super, Symbol* name, bool* created);
    Class*    open_class(State*, Module* under, Object* super, Symbol* name, bool* created);
    Module*   open_module(State*, Symbol* name);
    Module*   open_module(State*, Module* under, Symbol* name);

    bool yield_debugger(State*, Object* bp);
  };
}

#endif
