#ifndef RBX_BUILTIN_LOCATION_HPP
#define RBX_BUILTIN_LOCATION_HPP

#include "object_utils.hpp"
#include "memory.hpp"

#include "class/lexical_scope.hpp"
#include "class/fixnum.hpp"
#include "class/module.hpp"
#include "class/object.hpp"
#include "class/symbol.hpp"
#include "class/variable_scope.hpp"

namespace rubinius {

  class NativeMethodFrame;
  class LexicalScope;

  class Location : public Object {
  public:
    const static object_type type = LocationType;

    attr_accessor(method_module, Module);
    attr_accessor(receiver, Object);
    attr_accessor(name, Symbol);
    attr_accessor(method, Object);
    attr_accessor(ip, Fixnum);
    attr_accessor(flags, Fixnum);
    attr_accessor(variables, VariableScope);
    attr_accessor(lexical_scope, LexicalScope);

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
      obj->method_module(nil<Module>());
      obj->receiver(nil<Object>());
      obj->name(nil<Symbol>());
      obj->method(nil<Object>());
      obj->ip(nil<Fixnum>());
      obj->flags(nil<Fixnum>());
      obj->variables(nil<VariableScope>());
      obj->lexical_scope(nil<LexicalScope>());
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
