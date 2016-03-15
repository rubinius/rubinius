#ifndef RBX_BUILTIN_LOCATION_HPP
#define RBX_BUILTIN_LOCATION_HPP

#include "object_utils.hpp"
#include "memory.hpp"

#include "builtin/constant_scope.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/module.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"
#include "builtin/variable_scope.hpp"

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

    static void bootstrap(STATE);
    static void initialize(STATE, Location* obj) {
      obj->method_module_ = nil<Module>();
      obj->receiver_ = nil<Object>();
      obj->name_ = nil<Symbol>();
      obj->method_ = nil<Object>();
      obj->ip_ = nil<Fixnum>();
      obj->flags_ = nil<Fixnum>();
      obj->variables_ = nil<VariableScope>();
      obj->constant_scope_ = nil<ConstantScope>();
    }

    static Location* create(STATE, NativeMethodFrame* nmf);
    static Location* create(STATE, CallFrame* call_frame, bool include_variables=false);

    // Rubinius.primitive :location_allocate
    static Location* allocate(STATE, Object* self);

    // Rubinius.primitive :location_of_closest_ruby_method
    static Location* of_closest_ruby_method(STATE);

    static Array* debugging_call_stack(STATE);
    static Array* from_call_stack(STATE, ssize_t up=1);
    static Array* mri_backtrace(STATE, ssize_t up=1);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
