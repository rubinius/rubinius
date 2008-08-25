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

    struct jit_state js;
    int    ip;
    size_t args;
    OBJECT block; // slot
    OBJECT name; // slot

    size_t stack_size;
    // MUST BE AT THE LAST DATA MEMBER
    OBJECT stk[];

    static void init(STATE);
    static MethodContext* create(STATE, size_t stack_size);
    static MethodContext* create(STATE, OBJECT recv, CompiledMethod* meth);
    static void initialize_cache(STATE);
    static void reset_cache(STATE);

    int  line();
    bool recycle(STATE);
    void reference(STATE);

    // Ruby.primitive :context_get_field
    OBJECT get_field(STATE, FIXNUM type);

    /* Locals manipulation functions */
    /* Locals are just stored at the top of the stack. */
    void set_local(size_t pos, OBJECT val) {
      stk[pos] = val;
    }

    OBJECT get_local(size_t pos) {
      return stk[pos];
    }

    /* Stack manipulation functions */

    void clear_stack(size_t amount) {
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

    OBJECT top() {
      return *js.stack;
    }

    void set_top(OBJECT val) {
      *js.stack = val;
    }

    OBJECT stack_at(size_t pos) {
      return stk[pos];
    }

    void stack_put(size_t pos, OBJECT val) {
      stk[pos] = val;
    }

    void position_stack(int pos) {
      js.stack = stk + pos;
    }

    int calculate_sp() {
      return js.stack - stk;
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)

      virtual void mark(OBJECT, ObjectMark& mark);
    };
  };

  class BlockContext : public MethodContext {
    public:
    const static size_t fields = 0;
    const static object_type type = BContextType;

    BlockEnvironment* env();

    static BlockContext* create(STATE, size_t stack_size);

    class Info : public MethodContext::Info {
    public:
      BASIC_TYPEINFO(MethodContext::Info)
    };
  };


}

#endif
