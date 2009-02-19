#ifndef RBX_HELPERS_HPP
#define RBX_HELPERS_HPP

namespace rubinius {
  class CallFrame;
  class Module;
  class Class;
  class Symbol;
  class CompiledMethod;
  class VM;
  class Object;
  class LookupTableAssociation;

  namespace Helpers {
    void add_method(VM*, CallFrame* call_frame, Module* mod, Symbol* name, CompiledMethod* meth);
    void attach_method(VM*, CallFrame* call_frame, Object* obj, Symbol* name, CompiledMethod* meth);

    Object* const_get(VM*, Module* under, Symbol* name, bool* found);
    Object* const_get(VM*, CallFrame* call_frame, Symbol* name, bool* found);
    LookupTableAssociation* const_get_association(VM*, CallFrame* call_frame, Symbol* name, bool* found);
    Object* const_missing(VM*, Module* under, Symbol* sym, CallFrame* call_frame);
    void const_set(VM*, CallFrame* call_frame, Symbol* name, Object* val);
    void const_set(VM*, Module* mod, Symbol* name, Object* val);

    Object* locate_method_on(VM*, Object* recv, Symbol* name, Object* priv);
    Class* open_class(VM*, CallFrame* call_frame, Object* super, Symbol* name, bool* created);
    Class* open_class(VM*, CallFrame* call_frame, Module* under, Object* super, Symbol* name, bool* created);
    Module* open_module(VM*, CallFrame* call_frame, Symbol* name);
    Module* open_module(VM*, CallFrame* call_frame, Module* under, Symbol* name);

    void yield_debugger(VM*, CallFrame* call_frame);
  };
}

#endif
