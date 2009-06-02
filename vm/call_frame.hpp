#ifndef RBX_CALL_FRAME_HPP
#define RBX_CALL_FRAME_HPP

#include "vmmethod.hpp"
#include "unwind_info.hpp"
#include "jit_state.h"
#include "builtin/variable_scope.hpp"
#include "dispatch.hpp"

namespace rubinius {
  class Object;
  class Symbol;
  class Module;
  class VMMethod;
  class VariableScope;

  struct CallFrame {
    enum Flags {
      cIsLambda =           1 << 0,
      cCustomStaticScope =  1 << 1,
      cMultipleScopes =     1 << 2
    };

    CallFrame* previous;
    StaticScope* static_scope_;

    Dispatch* msg;
    CompiledMethod* cm;

    int flags;
    int args;
    int ip;

    VariableScope* top_scope_;
    VariableScope* scope;

    // Stack
    Object** stk;

    // ACCESS

    Symbol* name() {
      if(msg) return msg->name;
      return reinterpret_cast<Symbol*>(Qnil);
    }

    bool custom_static_scope_p() {
      return flags & cCustomStaticScope;
    }

    StaticScope* static_scope() {
      if(custom_static_scope_p()) return static_scope_;
      return cm->scope();
    }

    bool is_block_p(STATE) {
      return cm->name() == state->symbol("__block__");
    }

    Object* self() {
      return scope->self();
    }

    bool multiple_scopes_p() {
      return flags & cMultipleScopes;
    }

    VariableScope* top_scope() {
      if(multiple_scopes_p()) return top_scope_;
      return scope;
    }

    Module* module() {
      return top_scope()->module();
    }

    void set_ip(int new_ip) {
      ip = new_ip;
    }

    void promote_scope(STATE);

    void print_backtrace(STATE);
    int line(STATE);

    bool scope_still_valid(VariableScope* scope);
  };

  struct InterpreterCallFrame : public CallFrame {
    struct jit_state js;

    int current_unwind;
    UnwindInfo unwinds[kMaxUnwindInfos];


    // Methods

    /**
     *  Initialize frame for the given stack size.
     */
    void prepare(int stack) {
      ip = 0;
      current_unwind = 0;

      for(int i = 0; i < stack; i++) {
        stk[i] = Qnil;
      }

      js.stack = stk - 1;
      js.stack_top = (stk + stack) - 1;
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

    /**
     * Returns a pointer 'count' objects back on the stack.
     */
    Object** stack_back_position(size_t count) {
      return js.stack - (count - 1);
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

    // Manage the dynamic Unwind stack for this context
    void push_unwind(int target_ip, UnwindType type) {
      assert(current_unwind < kMaxUnwindInfos);
      UnwindInfo& info = unwinds[current_unwind++];
      info.target_ip = target_ip;
      info.stack_depth = calculate_sp();
      info.type = type;
    }

    UnwindInfo& pop_unwind() {
      assert(current_unwind > 0);
      return unwinds[--current_unwind];
    }

    bool has_unwinds_p() {
      return current_unwind > 0;
    }
  };
};

#endif
