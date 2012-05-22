#ifndef RBX_BUILTIN_LOCATION_HPP
#define RBX_BUILTIN_LOCATION_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"

namespace rubinius {

  class NativeMethodFrame;
  class ConstantScope;

  class Location : public Object {
    Module* method_module_; // slot
    Object* receiver_; // slot
    Symbol* name_; // slot
    Object* method_; // slot
    Fixnum* ip_; // slot
    Fixnum* flags_; // slot
    VariableScope* variables_; // slot
    ConstantScope* constant_scope_; // slot

  public:
    const static object_type type = LocationType;

    attr_accessor(method_module, Module);
    attr_accessor(receiver, Object);
    attr_accessor(name, Symbol);
    attr_accessor(method, Object);
    attr_accessor(ip, Fixnum);
    attr_accessor(flags, Fixnum);
    attr_accessor(variables, VariableScope);
    attr_accessor(constant_scope, ConstantScope);

    void set_is_block(STATE) {
      flags(state, Fixnum::from(flags()->to_native() | 1));
    }

    void set_is_jit(STATE) {
      flags(state, Fixnum::from(flags()->to_native() | 2));
    }

    void set_ip_on_current(STATE) {
      flags(state, Fixnum::from(flags()->to_native() | 4));
    }

    static void init(STATE);
    static Location* create(STATE, NativeMethodFrame* nmf);
    static Location* create(STATE, CallFrame* call_frame,
                            bool include_variables=false);

    // Rubinius.primitive :location_of_closest_ruby_method
    static Location* of_closest_ruby_method(STATE, CallFrame* calling_environment);

    static Array* from_call_stack(STATE, CallFrame* call_frame,
                                  bool include_vars=false, bool on_ip=false);
    static Array* mri_backtrace(STATE, CallFrame* call_frame);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
