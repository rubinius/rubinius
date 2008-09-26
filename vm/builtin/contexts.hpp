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
    // fields is 0 because it is dynamically calculated
    const static size_t fields = 0;
    const static object_type type = MethodContextType;

  private:
    MethodContext* sender_; // slot
    MethodContext* home_;   // slot
    OBJECT self_;           // slot

    CompiledMethod* cm_;    // slot

    Module* module_;        // slot
    OBJECT block_;          // slot
    OBJECT name_;           // slot

  public:
    // TODO: fix up data members that aren't slots
    VMMethod* vmm;

  public:
    // TODO: fix up data members that aren't slots
    struct jit_state js;
    int    ip;
    size_t args;

    size_t stack_size;
    // MUST BE AT THE LAST DATA MEMBER
    OBJECT stk[];

  public:
    /* accessors */

    attr_accessor(sender, MethodContext);
    attr_accessor(home, MethodContext);
    attr_accessor(self, Object);
    attr_accessor(cm, CompiledMethod);
    attr_accessor(module, Module);
    attr_accessor(block, Object);
    attr_accessor(name, Object);

    /* interface */

    static void init(STATE);
    static MethodContext* create(STATE, size_t stack_size);
    static MethodContext* create(STATE, OBJECT recv, CompiledMethod* meth);
    static void initialize_cache(STATE);
    static void reset_cache(STATE);

    int  line();
    bool recycle(STATE);
    void reference(STATE);

    // Ruby.primitive :context_get_field
    OBJECT get_internal_data(STATE, FIXNUM type);

    // Ruby.primitive :context_dup
    MethodContext* dup(STATE);

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
      sassert(js.stack < js.stack_top);
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

    /* Run after a context is copied, allowing it to fix up
     * its internal pointers */
    void post_copy(MethodContext* old);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)

      virtual void mark(OBJECT, ObjectMark& mark);
      virtual void show(STATE, OBJECT self, int level);
    };
  };

  class BlockContext : public MethodContext {
    public:
    // fields is 0 because it is dynamically calculated
    const static size_t fields = 0;
    const static object_type type = BlockContextType;

    BlockEnvironment* env();

    // Ruby.primitive :block_context_env
    BlockEnvironment* env(STATE);

    static BlockContext* create(STATE, size_t stack_size);

    class Info : public MethodContext::Info {
    public:
      BASIC_TYPEINFO(MethodContext::Info)
    };
  };


}

#endif
