#include "builtin/executable.hpp"

namespace rubinius {
  struct CallFrame;
  class Dispatch;
  class Arguments;
  class BlockEnvironment;

  class BlockAsMethod : public Executable {
  public:
    const static object_type type = BlockAsMethodType;

  private:
    BlockEnvironment* block_env_; // slot

  public:
    attr_accessor(block_env, BlockEnvironment);

    // Ruby.primitive :block_as_method_create
    static BlockAsMethod* create(STATE, Object* self, BlockEnvironment* be);

    static Object* block_executor(STATE, CallFrame* call_frame,
                           Dispatch& msg, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };
  };
}
