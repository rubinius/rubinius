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

  private:
    MethodContext* home_;       // slot
    MethodContext* home_block_; // slot
    OBJECT local_count_;        // slot
    CompiledMethod* method_;    // slot

  public:
    // TODO: fix up data members that aren't slots
    VMMethod* vmm;

  public:
    /* accessors */
    attr_accessor(home, MethodContext);
    attr_accessor(home_block, MethodContext);
    attr_accessor(local_count, Object);
    attr_accessor(method, CompiledMethod);

    /* interface */

    static void init(STATE);
    static BlockEnvironment* under_context(STATE, CompiledMethod* cm,
        MethodContext* parent, MethodContext* active, size_t index);

    void call(STATE, Task* task, size_t args);
    void call(STATE, Task* task, Message& msg);
    BlockContext* create_context(STATE, MethodContext* sender);

    // Ruby.primitive? :block_call
    bool call_prim(STATE, Executable* exec, Task* task, Message& msg);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, OBJECT self, int level);
    };
  };
};

#endif
