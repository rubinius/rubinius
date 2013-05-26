#ifndef RBX_BUILTIN_BLOCK_WRAPPER_HPP
#define RBX_BUILTIN_BLOCK_WRAPPER_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class BlockEnvironment;

  class Proc : public Object {
  public:
    const static object_type type = ProcType;

  private:
    BlockEnvironment* block_; // slot
    Object* lambda_; // slot
    Object* bound_method_; // slot
    Object* ruby_method_; // slot

  public:
    attr_accessor(block, BlockEnvironment);
    attr_accessor(lambda, Object);
    attr_accessor(bound_method, Object);
    attr_accessor(ruby_method, Object);

    static void init(STATE);

    // Rubinius.primitive :proc_allocate
    static Proc* create(STATE, Object* self);

    Object* yield(STATE, CallFrame* call_frame, Arguments& args);

    Object* call(STATE, CallFrame* call_frame, Arguments& args);

    // Rubinius.primitive? :proc_call
    Object* call_prim(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive? :proc_call_on_object
    Object* call_on_object(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

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
