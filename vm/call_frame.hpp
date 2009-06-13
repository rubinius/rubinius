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

  class InterpreterCallFrame : public CallFrame {
  public:
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
    }

    /* Stack manipulation functions */

    // Manage the dynamic Unwind stack for this context
    void push_unwind(int target_ip, UnwindType type, int sp) {
      assert(current_unwind < kMaxUnwindInfos);
      UnwindInfo& info = unwinds[current_unwind++];
      info.target_ip = target_ip;
      info.stack_depth = sp;
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
