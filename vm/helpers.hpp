#ifndef RBX_HELPERS_HPP
#define RBX_HELPERS_HPP

namespace rubinius {
  struct CallFrame;
  class Module;
  class Class;
  class Symbol;
  class CompiledMethod;
  class VM;
  class Object;
  class GlobalCacheEntry;
  class Tuple;

  namespace Helpers {
    Object* const_get_under(VM*, Module* under, Symbol* name, bool* found);
    Object* const_get(VM*, CallFrame* call_frame, Symbol* name, bool* found);
    Object* const_missing_under(VM*, Module* under, Symbol* sym, CallFrame* call_frame);
    Object* const_missing(VM*, Symbol* sym, CallFrame* call_frame);
    void const_set(VM*, CallFrame* call_frame, Symbol* name, Object* val);
    void const_set(VM*, Module* mod, Symbol* name, Object* val);

    /** Locate method just as if sending. */
    Tuple*    locate_method_on(VM*, CallFrame* call_frame, Object* recv, Symbol* name, Object* priv);

    Class*    open_class(VM*, CallFrame* call_frame, Object* super, Symbol* name, bool* created);
    Class*    open_class(VM*, CallFrame* call_frame, Module* under, Object* super, Symbol* name, bool* created);
    Module*   open_module(VM*, CallFrame* call_frame, Symbol* name);
    Module*   open_module(VM*, CallFrame* call_frame, Module* under, Symbol* name);

    bool yield_debugger(VM*, CallFrame* call_frame, Object* bp);
  };
}

#endif
