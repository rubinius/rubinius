#ifndef RBX_BUILTIN_BLOCK_WRAPPER_HPP
#define RBX_BUILTIN_BLOCK_WRAPPER_HPP

#include "object_utils.hpp"

#include "class/block_environment.hpp"
#include "class/object.hpp"

namespace rubinius {
  class BlockEnvironment;

  class Proc : public Object {
  public:
    const static object_type type = ProcType;

    attr_accessor(block, BlockEnvironment);
    attr_accessor(lambda, Object);
    attr_accessor(bound_method, Object);
    attr_accessor(ruby_method, Object);

    static void bootstrap(STATE);
    static void initialize(STATE, Proc* obj) {
      obj->block(nil<BlockEnvironment>());
      obj->lambda(nil<Object>());
      obj->bound_method(nil<Object>());
      obj->ruby_method(nil<Object>());
    }

    // Rubinius.primitive :proc_allocate
    static Proc* create(STATE, Object* self);

    Object* yield(STATE, Arguments& args);

    Object* call(STATE, Arguments& args);

    // Rubinius.primitive? :proc_call
    Object* call_prim(STATE, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive? :proc_call_on_object
    Object* call_on_object(STATE, Executable* exec, Module* mod, Arguments& args);

    static Proc* from_env(STATE, Object* self, Object* env);

    // Rubinius.primitive :proc_from_env
    static Proc* from_env_prim(STATE, Object* self, Object* env);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
