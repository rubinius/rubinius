#ifndef RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP
#define RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class CompiledMethod;
  class MethodContext;
  class BlockContext;
  class Message;
  class VMMethod;
  class Task;
  class VMExecutable;

  class BlockEnvironment : public Object {
    public:
    const static size_t fields = 5;
    const static object_type type = BlockEnvType;

    MethodContext* home; // slot
    MethodContext* home_block; // slot
    OBJECT local_count; // slot
    CompiledMethod* method; // slot
    VMMethod* vmm;

    static void init(STATE);
    static BlockEnvironment* under_context(STATE, CompiledMethod* cm,
        MethodContext* parent, MethodContext* active, size_t index);

    void call(STATE, Task* task, size_t args);
    void call(STATE, Task* task, Message& msg);
    BlockContext* create_context(STATE, MethodContext* sender);

    // Ruby.primitive? :block_call
    bool call_prim(STATE, VMExecutable* exec, Task* task, Message& msg);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
