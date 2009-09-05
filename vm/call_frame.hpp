#ifndef RBX_CALL_FRAME_HPP
#define RBX_CALL_FRAME_HPP

#include "vmmethod.hpp"
#include "unwind_info.hpp"
#include "jit_state.h"
#include "stack_variables.hpp"
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
      cMultipleScopes =     1 << 2,
      cInlineFrame =        1 << 3,
      cClosedScope =        1 << 4
    };

    CallFrame* previous;
    StaticScope* static_scope_;

    Dispatch* msg;
    CompiledMethod* cm;

    int flags;
    int ip_;

    VariableScope* top_scope_;
    StackVariables* scope;

    // Stack
    Object* stk[];

    // ACCESS

    int ip() {
      return ip_;
    }

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

    VariableScope* top_scope(STATE) {
      if(multiple_scopes_p()) return top_scope_;
      return promote_scope(state);
    }

    bool is_inline_frame() {
      return flags & cInlineFrame;
    }

    bool has_closed_scope_p() {
      return flags & cClosedScope;
    }

    Module* module() {
      if(multiple_scopes_p()) {
        return top_scope_->module();
      } else {
        return scope->module();
      }
    }

    void set_ip(int new_ip) {
      ip_ = new_ip;
    }

    int dec_ip() {
      return ip_--;
    }

    int inc_ip() {
      return ip_++;
    }

    void calculate_ip(void** pos) {
      ip_ = pos - cm->backend_method()->addresses;
    }

    VariableScope* promote_scope(STATE);

    void print_backtrace(STATE);
    int line(STATE);

    bool scope_still_valid(VariableScope* scope);

    void dump();
  };

  class InterpreterCallFrame : public CallFrame {
  public:
    // Methods

    /**
     *  Initialize frame for the given stack size.
     */
    void prepare(int stack) {
      ip_ = 0;

      for(int i = 0; i < stack; i++) {
        stk[i] = Qnil;
      }
    }
  };

#define ALLOCA_CALLFRAME(stack_size) \
  (InterpreterCallFrame*)alloca(sizeof(InterpreterCallFrame) + (sizeof(Object*) * stack_size))
};

#endif
