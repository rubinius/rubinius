#ifndef RBX_BUILTIN_BLOCK_AS_METHOD_HPP
#define RBX_BUILTIN_BLOCK_AS_METHOD_HPP

#include "object_utils.hpp"

#include "class/block_environment.hpp"
#include "class/executable.hpp"

namespace rubinius {
  struct CallFrame;
  class Dispatch;
  class Arguments;
  class BlockEnvironment;

  class BlockAsMethod : public Executable {
  public:
    const static object_type type = BlockAsMethodType;

    attr_accessor(block_env, BlockEnvironment);

    static void initialize(STATE, BlockAsMethod* obj) {
      Executable::initialize(state, obj);

      obj->block_env(nil<BlockEnvironment>());
    }

    // Rubinius.primitive :block_as_method_create
    static BlockAsMethod* create(STATE, Object* self, BlockEnvironment* be);

    static Object* block_executor(STATE,
                           Executable* exec, Module* mod, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}

#endif
