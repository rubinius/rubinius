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
    const static object_type type = MethodContextType;

  private:
    MethodContext* sender_; // slot

    // initialized in ::execute_specialized
    Module* module_;        // slot

    // initialized in ::execute_specialized
    Object* block_;          // slot

    // initialized in ::execute_specialized
    Object* name_;           // slot

    // Public slots. These have to be public because we access
    // them directly via machine code.
  public:
    // initialized in ::create
    MethodContext* home_;   // slot

    // initialized in ::create
    CompiledMethod* cm_;    // slot


    // initialized in ::create
    Object* self_;           // slot

  public:
    // @todo fix up data members that aren't slots
    VMMethod* vmm;

  public:
    // @todo fix up data members that aren't slots
    struct jit_state js;
    int    ip;
    void*  native_ip;
    size_t args;

    size_t full_size;
    size_t stack_size;
    // MUST BE AT THE LAST DATA MEMBER
    Object* stk[];

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
    static MethodContext* create(STATE, Object* recv, CompiledMethod* meth);
    static void initialize_cache(STATE);
    static void reset_cache(STATE);

    int  line(STATE);
    bool recycle(STATE);
    void initialize_as_reference(STATE);
    void reference(STATE);

    // Ruby.primitive :context_get_field
    Object* get_internal_data(STATE, Fixnum* type);

    // Ruby.primitive :context_dup
    MethodContext* dup(STATE);

    /* Locals manipulation functions */
    /* Locals are just stored at the top of the stack. */
    void set_local(size_t pos, Object* val) {
      stk[pos] = val;
    }

    Object* get_local(size_t pos) {
      return stk[pos];
    }

    /* Stack manipulation functions */

    void clear_stack(size_t amount) {
      js.stack -= amount;
#ifdef EXTRA_STACK_PROTECTION
      if(amount > 0) {
        assert(js.stack >= stk - 1);
      }
#endif
    }

    Object* pop() {
#ifdef EXTRA_STACK_PROTECTION
      assert(js.stack >= stk);
#endif
      return *js.stack--;
    }

    void push(Object* value) {
      *++js.stack = value;
#ifdef EXTRA_STACK_PROTECTION
      assert(js.stack < js.stack_top);
#endif
    }

    Object* stack_back(size_t position) {
      Object** pos = js.stack - position;
      return *pos;
    }

    Object** stack_back_position(size_t position) {
      return js.stack - position;
    }

    Object* top() {
      return *js.stack;
    }

    void set_top(Object* val) {
      *js.stack = val;
    }

    Object* stack_at(size_t pos) {
      return stk[pos];
    }

    void stack_put(size_t pos, Object* val) {
      stk[pos] = val;
    }

    void position_stack(int pos) {
      js.stack = stk + pos;
#ifdef EXTRA_STACK_PROTECTION
      if(pos > 0) {
        assert(js.stack >= stk && js.stack < js.stack_top);
      }
#endif
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

      virtual void mark(Object*, ObjectMark& mark);
      virtual void show(STATE, Object* self, int level);
    };
  };

  class BlockContext : public MethodContext {
    public:
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
