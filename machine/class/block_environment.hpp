#ifndef RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP
#define RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP

#include "executor.hpp"
#include "object_utils.hpp"

#include "class/compiled_code.hpp"
#include "class/lexical_scope.hpp"
#include "class/module.hpp"
#include "class/object.hpp"
#include "class/string.hpp"
#include "class/variable_scope.hpp"

namespace rubinius {
  class CompiledCode;
  class VariableScope;
  struct CallFrame;
  class MachineCode;
  class VMExecutable;

  class BlockEnvironment;
  class LexicalScope;

  struct BlockInvocation {
    int flags;
    Object* self;
    LexicalScope* lexical_scope;
    Module* module;

    BlockInvocation(Object* self, LexicalScope* lexical_scope, int flags)
      : flags(flags)
      , self(self)
      , lexical_scope(lexical_scope)
      , module(0)
    {}
  };

  typedef Object* (*BlockExecutor)(STATE, BlockEnvironment* const,
                                   Arguments&, BlockInvocation& invocation);

  class BlockEnvironment : public Object {
  public:
    const static object_type type = BlockEnvironmentType;

    attr_accessor(scope, VariableScope);
    attr_accessor(top_scope, VariableScope);
    attr_accessor(compiled_code, CompiledCode);
    attr_accessor(lexical_scope, LexicalScope);
    attr_accessor(module, Module);

  private:
    MachineCode* machine_code(STATE);

  public:
    static void bootstrap(STATE);
    static void bootstrap_methods(STATE);

    static void initialize(STATE, BlockEnvironment* obj) {
      obj->scope(nil<VariableScope>());
      obj->top_scope(nil<VariableScope>());
      obj->compiled_code(nil<CompiledCode>());
      obj->lexical_scope(nil<LexicalScope>());
      obj->module(nil<Module>());
    }

    // Rubinius.primitive :blockenvironment_allocate
    static BlockEnvironment* allocate(STATE);

    static Object* invoke(STATE, BlockEnvironment* env, Arguments& args,
                          BlockInvocation& invocation);

    static BlockEnvironment* under_call_frame(STATE, CompiledCode* cm, MachineCode* caller);

    static Object* execute_interpreter(STATE,
                            BlockEnvironment* env, Arguments& args,
                            BlockInvocation& invocation);

    Object* call(STATE, Arguments& args, int flags=0);

    // Rubinius.primitive? :block_call
    Object* call_prim(STATE, Executable* exec, Module* mod, Arguments& args);

    Object* call_on_object(STATE, Arguments& args, int flags=0);

    // Rubinius.primitive? :block_call_under
    Object* call_under(STATE, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive :block_env_of_sender
    static Object* of_sender(STATE);

    void lock_scope(STATE);

    BlockEnvironment* dup(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
};

#endif
