#ifndef RBX_CALL_FRAME_HPP
#define RBX_CALL_FRAME_HPP

#include "vmmethod.hpp"
#include "unwind_info.hpp"
#include "jit_state.h"
#include "stack_variables.hpp"
#include "builtin/variable_scope.hpp"
#include "dispatch.hpp"

#ifdef ENABLE_LLVM
#include "llvm/jit_runtime.hpp"
#endif

namespace rubinius {
  class Object;
  class Symbol;
  class Module;
  class VMMethod;
  class VariableScope;

  namespace jit {
    class RuntimeData;
    class RuntimeDataHolder;
  }

  struct CallFrame {
    enum Flags {
      cIsLambda =           1 << 0,
      cCustomStaticScope =  1 << 1,
      cMultipleScopes =     1 << 2,
      cInlineFrame =        1 << 3,
      cClosedScope =        1 << 4,
      cBlockAsMethod =      1 << 5,
      cJITed =              1 << 6,
      cBlock =              1 << 7
    };

    CallFrame* previous;
    StaticScope* static_scope_;

    void* dispatch_data;
    CompiledMethod* cm;

    int flags;
    int ip_;

    void* optional_jit_data;
    VariableScope* top_scope_;
    StackVariables* scope;

    Arguments* arguments;

    // Stack
    Object* stk[];

    // ACCESS

    int ip() {
      return ip_;
    }

    bool block_as_method_p() {
      return flags & cBlockAsMethod;
    }

#ifdef ENABLE_LLVM
    Symbol* name() {
      if(dispatch_data) {
        if(inline_method_p()) {
          return reinterpret_cast<jit::RuntimeData*>(dispatch_data)->name();
        } else {
          return reinterpret_cast<Dispatch*>(dispatch_data)->name;
        }
      }

      return reinterpret_cast<Symbol*>(Qnil);
    }
#else
    Symbol* name() {
      if(dispatch_data) {
        return reinterpret_cast<Dispatch*>(dispatch_data)->name;
      }

      return reinterpret_cast<Symbol*>(Qnil);
    }
#endif

    Dispatch* dispatch() {
      if(dispatch_data) {
        if(!inline_method_p()) {
          return reinterpret_cast<Dispatch*>(dispatch_data);
        }
      }

      return NULL;
    }

#ifdef ENABLE_LLVM
    jit::RuntimeData* runtime_data() {
      if(dispatch_data) {
        if(inline_method_p()) {
          return reinterpret_cast<jit::RuntimeData*>(dispatch_data);
        }
      }

      return NULL;
    }

    jit::RuntimeDataHolder* jit_data() {
      if(jitted_p()) {
        return reinterpret_cast<jit::RuntimeDataHolder*>(optional_jit_data);
      }

      return NULL;
    }
#else
    jit::RuntimeData* runtime_data() {
      return NULL;
    }

    jit::RuntimeDataHolder* jit_data() {
      return NULL;
    }
#endif

    Symbol* original_name() {
      if(multiple_scopes_p()) {
        if(block_as_method_p()) return cm->name();
        return top_scope_->method()->name();
      }

      return cm->name();
    }

    bool custom_static_scope_p() {
      return flags & cCustomStaticScope;
    }

    bool inline_method_p() {
      return flags & cInlineFrame;
    }

    bool jitted_p() {
      return flags & cJITed;
    }

    bool block_p() {
      return flags & cBlock;
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
      return scope->module();
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

    VariableScope* promote_scope_full(STATE);

    VariableScope* promote_scope(STATE) {
      if(VariableScope* vs = scope->on_heap()) return vs;
      return promote_scope_full(state);
    }

    VariableScope* method_scope(STATE);

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
