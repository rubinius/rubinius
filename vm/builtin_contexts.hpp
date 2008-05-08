#ifndef RBX_BUILTIN_CONTEXTS_HPP
#define RBX_BUILTIN_CONTEXTS_HPP

#include "objects.hpp"
#include "vmmethod.hpp"

namespace rubinius {
  class BlockContext;
  class MethodContext;

  class MethodContext : public BuiltinType {
    public:
    const static size_t fields = 0;
    const static object_type type = MContextType;

    MethodContext* sender;
    OBJECT self;

    CompiledMethod* cm;
    VMMethod* vmm;

    Module* module;

    Tuple* stack;

    size_t ip;
    int    sp;
    size_t args;
    OBJECT block;
    OBJECT name;
    bool   no_value;

    /* Locals are stored at the top of the stack. */
    Tuple* locals() { return stack; }

    static MethodContext* create(STATE);
    void reference(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(MethodContext* obj);
    };
  };

  class BlockContext : public MethodContext {
    public:
    const static size_t fields = 0;
    const static object_type type = BContextType;

    BlockEnvironment* env() {
      return as<BlockEnvironment>(name);
    }

    static BlockContext* create(STATE);

    class Info : public MethodContext::Info {
    public:
      Info(object_type type) : MethodContext::Info(type) { }
    };
  };

  template <>
    static bool kind_of<MethodContext>(OBJECT obj) {
      return obj->obj_type == MethodContext::type ||
        obj->obj_type == BlockContext::type;
    }

}

#endif
