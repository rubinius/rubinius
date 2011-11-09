#ifndef RBX_HELPERS_HPP
#define RBX_HELPERS_HPP

namespace rubinius {
  struct CallFrame;
  class Module;
  class Class;
  class Symbol;
  class CompiledMethod;
  class State;
  class Object;
  class GlobalCacheEntry;
  class Tuple;

  namespace Helpers {
    Object* const_get_under(State*, Module* under, Symbol* name, bool* found);
    Object* const_get(State*, CallFrame* call_frame, Symbol* name, bool* found);
    Object* const_missing_under(State*, Module* under, Symbol* sym, CallFrame* call_frame);
    Object* const_missing(State*, Symbol* sym, CallFrame* call_frame);
    void const_set(State*, CallFrame* call_frame, Symbol* name, Object* val);
    void const_set(State*, Module* mod, Symbol* name, Object* val);

    /** Locate method just as if sending. */
    Tuple*    locate_method_on(State*, CallFrame* call_frame, Object* recv, Symbol* name, Object* priv);

    Class*    open_class(State*, CallFrame* call_frame, Object* super, Symbol* name, bool* created);
    Class*    open_class(State*, CallFrame* call_frame, Module* under, Object* super, Symbol* name, bool* created);
    Module*   open_module(State*, CallFrame* call_frame, Symbol* name);
    Module*   open_module(State*, CallFrame* call_frame, Module* under, Symbol* name);

    bool yield_debugger(State*, GCToken gct, CallFrame* call_frame, Object* bp);
  };
}

#endif
