#ifndef RBX_BUILTIN_CONTEXTS_HPP
#define RBX_BUILTIN_CONTEXTS_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"
#include "vmmethod.hpp"
#include "jit_state.h"

namespace rubinius {
  class BlockContext;
  class MethodContext;
  class BlockEnvironment;
  class CompiledMethod;
  class VMMethod;
  class Tuple;
  class Module;

  class MethodContext : public Object {
    public:
    const static size_t fields = 0;
    const static object_type type = MContextType;

    MethodContext* sender; // slot
    MethodContext* home; // slot
    OBJECT self; // slot

    CompiledMethod* cm; // slot
    VMMethod* vmm;

    Module* module; // slot

    Tuple* stack; // slot

    struct jit_state js;
    int    ip;
    int    sp;
    size_t args;
    OBJECT block; // slot
    OBJECT name; // slot

    /* Locals are stored at the top of the stack. */
    Tuple* locals() { return stack; }

    static MethodContext* create(STATE);
    void reference(STATE);

    /* Stack manipulation functions */

    void clear_stack(size_t amount) {
      sp -= amount;
      js.stack -= amount;
    }

    OBJECT pop() {
      return *js.stack--;
    }

    void push(OBJECT value) {
      *++js.stack = value;
    }

    OBJECT stack_back(size_t position) {
      OBJECT* pos = js.stack - position;
      return *pos;
    }

    void set_top(OBJECT val) {
      *js.stack = val;
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class BlockContext : public MethodContext {
    public:
    const static size_t fields = 0;
    const static object_type type = BContextType;

    BlockEnvironment* env();

    static BlockContext* create(STATE);

    class Info : public MethodContext::Info {
    public:
      BASIC_TYPEINFO(MethodContext::Info)
    };
  };

  template <>
    static bool kind_of<MethodContext>(OBJECT obj) {
      return obj->obj_type == MethodContext::type ||
        obj->obj_type == BlockContext::type;
    }

}

#endif
