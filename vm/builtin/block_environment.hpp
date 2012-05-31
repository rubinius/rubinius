#ifndef RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP
#define RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"
#include "executor.hpp"

namespace rubinius {
  class CompiledMethod;
  class VariableScope;
  struct CallFrame;
  class VMMethod;
  class VMExecutable;

  class BlockEnvironment;
  class ConstantScope;

  struct BlockInvocation {
    int flags;
    Object* self;
    ConstantScope* constant_scope;
    Module* module;

    BlockInvocation(Object* self, ConstantScope* constant_scope, int flags)
      : flags(flags)
      , self(self)
      , constant_scope(constant_scope)
      , module(0)
    {}
  };

  typedef Object* (*BlockExecutor)(STATE, CallFrame*, BlockEnvironment* const,
                                   Arguments&, BlockInvocation& invocation);

  class BlockEnvironment : public Object {
  public:
    const static object_type type = BlockEnvironmentType;

  private:
    VariableScope* scope_;      // slot
    VariableScope* top_scope_;  // slot
    CompiledMethod* code_;      // slot
    Module* module_;            // slot

  public:
    /* accessors */
    attr_accessor(scope, VariableScope);
    attr_accessor(top_scope, VariableScope);
    attr_accessor(code, CompiledMethod);
    attr_accessor(module, Module);

    /* interface */

    static void init(STATE);
    VMMethod* vmmethod(STATE, GCToken gct);

    // Rubinius.primitive :blockenvironment_allocate
    static BlockEnvironment* allocate(STATE);

    static Object* invoke(STATE, CallFrame* previous,
                            BlockEnvironment* env, Arguments& args,
                            BlockInvocation& invocation);

    static BlockEnvironment* under_call_frame(STATE, GCToken gct, CompiledMethod* cm,
      VMMethod* caller, CallFrame* call_frame);

    static Object* execute_interpreter(STATE, CallFrame* previous,
                            BlockEnvironment* env, Arguments& args,
                            BlockInvocation& invocation);

    Object* call(STATE, CallFrame* call_frame, Arguments& args, int flags=0);

    // Rubinius.primitive? :block_call
    Object* call_prim(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    Object* call_on_object(STATE, CallFrame* call_frame, Arguments& args, int flags=0);

    // Rubinius.primitive? :block_call_under
    Object* call_under(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive :block_env_of_sender
    static Object* of_sender(STATE, CallFrame* calling_environment);

    BlockEnvironment* dup(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
};

#endif
