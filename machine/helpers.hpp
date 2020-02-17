#ifndef RBX_HELPERS_HPP
#define RBX_HELPERS_HPP

#include "thread_state.hpp"

namespace rubinius {
  struct CallFrame;
  class Module;
  class Class;
  class Symbol;
  class CompiledCode;
  class ThreadState;
  class Object;
  class Tuple;

  namespace Helpers {
    Object* const_get_under(ThreadState*, Module* under, Symbol* name, ConstantMissingReason* reason, Object* filter = NULL, bool replace_autoload = false);
    Object* const_get(ThreadState*, Symbol* name, ConstantMissingReason* reason, Object* filter = NULL, bool replace_autoload = false);
    Object* const_missing_under(ThreadState*, Module* under, Symbol* sym);
    Object* const_missing(ThreadState*, Symbol* sym);
    void const_set(ThreadState*, Symbol* name, Object* val);
    void const_set(ThreadState*, Module* mod, Symbol* name, Object* val);

    Class*    open_class(ThreadState*, Object* super, Symbol* name, bool* created);
    Class*    open_class(ThreadState*, Module* under, Object* super, Symbol* name, bool* created);
    Module*   open_module(ThreadState*, Symbol* name);
    Module*   open_module(ThreadState*, Module* under, Symbol* name);
  };
}

#endif
