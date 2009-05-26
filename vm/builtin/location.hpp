#ifndef RBX_BUILTIN_LOCATION_HPP
#define RBX_BUILTIN_LOCATION_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class Location : public Object {
    Module* method_module_; // slot
    Object* receiver_; // slot
    Symbol* name_; // slot
    Object* method_; // slot
    Object* is_block_; // slot
    Fixnum* ip_; // slot
    Object* is_jit_; // slot

  public:
    const static object_type type = LocationType;

    attr_accessor(method_module, Module);
    attr_accessor(receiver, Object);
    attr_accessor(name, Symbol);
    attr_accessor(method, Object);
    attr_accessor(is_block, Object);
    attr_accessor(ip, Fixnum);
    attr_accessor(is_jit, Object);

    static void init(STATE);
    static Location* create(STATE, CallFrame* call_frame);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
