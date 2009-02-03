#ifndef RBX_BUILTIN_BLOCK_WRAPPER_HPP
#define RBX_BUILTIN_BLOCK_WRAPPER_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class BlockEnvironment;

  class BlockWrapper : public Object {
  public:
    const static object_type type = BlockWrapperType;

  private:
    BlockEnvironment* block_; // slot

  public:
    attr_accessor(block, BlockEnvironment);

    static void init(STATE);

    // Ruby.primitive :block_wrapper_allocate
    static BlockWrapper* create(STATE, Object* self);

    Object* call(STATE, Task* task, CallFrame* call_frame, size_t args);

    // Ruby.primitive? :block_wrapper_call
    Object* call_prim(STATE, Executable* exec, CallFrame* call_frame, Task* task, Message& msg);

    // Ruby.primitive :block_wrapper_from_env
    static BlockWrapper* from_env(STATE, BlockEnvironment* env);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
}

#endif
