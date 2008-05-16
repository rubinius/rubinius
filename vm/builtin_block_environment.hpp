#ifndef RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP
#define RBX_BUILTIN_BLOCK_ENVIRONMENT_HPP

namespace rubinius {
  class CompiledMethod;
  class MethodContext;
  class BlockContext;
  class Message;

  class BlockEnvironment : public BuiltinType {
    public:
    const static size_t fields = 5;
    const static object_type type = BlockEnvType;

    OBJECT __ivars__; // slot
    MethodContext* home; // slot
    MethodContext* home_block; // slot
    OBJECT local_count; // slot
    CompiledMethod* method; // slot

    static BlockEnvironment* under_context(STATE, CompiledMethod* cm,
        MethodContext* parent, MethodContext* active);

    void call(STATE, size_t args);
    void call(STATE, Message& msg);
    BlockContext* create_context(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };
};

#endif
