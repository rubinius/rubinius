#ifndef RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP
#define RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class CompiledMethod;
  class VariableScope;
  class CallFrame;
  class Message;
  class VMMethod;
  class VMExecutable;

  class BlockEnvironment : public Object {
  public:
    const static object_type type = BlockEnvironmentType;

  private:
    VariableScope* scope_;      // slot
    VariableScope* top_scope_;  // slot
    Object* local_count_;       // slot
    CompiledMethod* method_;    // slot

  public:
    // @todo fix up data members that aren't slots
    VMMethod* vmm;

  public:
    /* accessors */
    attr_accessor(scope, VariableScope);
    attr_accessor(top_scope, VariableScope);
    attr_accessor(local_count, Object);
    attr_accessor(method, CompiledMethod);

    /* interface */

    static void init(STATE);

    // Ruby.primitive :blockenvironment_allocate
    static BlockEnvironment* allocate(STATE);

    static BlockEnvironment* under_call_frame(STATE, CompiledMethod* cm,
      VMMethod* caller, CallFrame* call_frame, size_t index);

    Object* call(STATE, CallFrame* call_frame, Arguments& args, int flags=0);

    // Ruby.primitive? :block_call
    Object* call_prim(STATE, Executable* exec, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    Object* call_on_object(STATE, CallFrame* call_frame, Arguments& args, int flags=0);

    // Ruby.primitive? :block_call_under
    Object* call_under(STATE, Executable* exec, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    BlockEnvironment* dup(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
};

#endif
