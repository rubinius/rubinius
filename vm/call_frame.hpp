#ifndef RBX_CALL_FRAME_HPP
#define RBX_CALL_FRAME_HPP

#include "machine_code.hpp"
#include "unwind_info.hpp"
#include "stack_variables.hpp"
#include "dispatch.hpp"
#include "arguments.hpp"
#include "object_utils.hpp"
#include "builtin/symbol.hpp"

#include <ostream>

#ifdef ENABLE_LLVM
#include "llvm/jit_runtime.hpp"
#endif

namespace rubinius {
  class Module;
  class VariableScope;
  class NativeMethodFrame;
  class BlockEnvironment;

  namespace jit {
    class RuntimeData;
    class RuntimeDataHolder;
  }

  struct CallFrame {
    enum Flags {
      cIsLambda =           1 << 0,
      cMultipleScopes =     1 << 1,
      cInlineFrame =        1 << 2,
      cClosedScope =        1 << 3,
      cBlockAsMethod =      1 << 4,
      cJITed =              1 << 5,
      cBlock =              1 << 6,
      cInlineBlock =        1 << 7,
      cNativeMethod =       1 << 8,
      cTopLevelVisibility = 1 << 9,
      cScript =             1 << 10,
      cScopeLocked =        1 << 11
    };

    CallFrame* previous;
    ConstantScope* constant_scope_;

    void* dispatch_data;
    CompiledCode* compiled_code;

    int flags;
    int ip_;

    void* optional_jit_data;
    VariableScope* top_scope_;
    StackVariables* scope;

    Arguments* arguments;

    // Stack
    Object* stk[];

    // ACCESS

    int ip() const {
      return ip_;
    }

    bool block_as_method_p() const {
      return flags & cBlockAsMethod;
    }

    BlockEnvironment* block_env() {
      if(block_p()) {
        return reinterpret_cast<BlockEnvironment*>(dispatch_data);
      }

      return NULL;
    }

    void set_block_env(BlockEnvironment* env) {
      if(block_p()) {
        dispatch_data = reinterpret_cast<void*>(env);
      }
    }

#ifdef ENABLE_LLVM
    Symbol* name() const {
      if(inline_method_p() && dispatch_data) {
        return reinterpret_cast<jit::RuntimeData*>(dispatch_data)->name();
      } else if(block_p()) {
        return nil<Symbol>();
      } else if(arguments) {
        return arguments->name();
      }

      return nil<Symbol>();
    }
#else
    Symbol* name() const {
      if(arguments && !block_p()) {
        return arguments->name();
      }

      return nil<Symbol>();
    }
#endif

#ifdef ENABLE_LLVM
    jit::RuntimeData* runtime_data() const {
      if(dispatch_data) {
        if(inline_method_p()) {
          return reinterpret_cast<jit::RuntimeData*>(dispatch_data);
        }
      }

      return NULL;
    }

    jit::RuntimeDataHolder* jit_data() const {
      if(jitted_p()) {
        return reinterpret_cast<jit::RuntimeDataHolder*>(optional_jit_data);
      }

      return NULL;
    }
#else
    jit::RuntimeData* runtime_data() const {
      return NULL;
    }

    jit::RuntimeDataHolder* jit_data() const {
      return NULL;
    }
#endif

    Symbol* original_name() const {
      return compiled_code->name();
    }

    bool inline_method_p() const {
      return flags & cInlineFrame;
    }

    bool jitted_p() const {
      return flags & cJITed;
    }

    bool block_p() const {
      return flags & cBlock;
    }

    ConstantScope* constant_scope() const {
      return constant_scope_;
    }

    bool is_block_p(STATE) const {
      return block_p();
    }

    Object* self() const {
      return scope->self();
    }

    bool multiple_scopes_p() const {
      return flags & cMultipleScopes;
    }

    VariableScope* top_scope(STATE) {
      if(multiple_scopes_p()) return top_scope_;
      return promote_scope(state);
    }

    bool is_inline_frame() const {
      return flags & cInlineFrame;
    }

    bool is_inline_block() const {
      return flags & cInlineBlock;
    }

    bool has_closed_scope_p() const {
      return flags & cClosedScope;
    }

    bool native_method_p() const {
      return flags & cNativeMethod;
    }

    NativeMethodFrame* native_method_frame() const {
      if(native_method_p()) return reinterpret_cast<NativeMethodFrame*>(dispatch_data);
      return 0;
    }

    CallFrame* top_ruby_frame() {
      // Skip over any native method frames.
      CallFrame* cf = this;

      while(cf->native_method_p()) {
        cf = cf->previous;
      }

      return cf;
    }

    Object* last_match(STATE);
    void set_last_match(STATE, Object* obj);

    Module* module() const {
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

    VariableScope* promote_scope_full(STATE);

    VariableScope* promote_scope(STATE) {
      if(VariableScope* vs = scope->on_heap()) return vs;
      return promote_scope_full(state);
    }

    VariableScope* method_scope(STATE);

    void print_backtrace(STATE, int count=0, bool filter=false);
    void print_backtrace(STATE, std::ostream& stream, int count=0, bool filter=false);

    Symbol* file(STATE);
    int line(STATE);

    bool scope_still_valid(VariableScope* scope);

    void dump();

    Object* find_breakpoint(STATE);
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
        stk[i] = cNil;
      }
    }
  };

#define ALLOCA_CALLFRAME(stack_size) \
  reinterpret_cast<InterpreterCallFrame*>(alloca(sizeof(InterpreterCallFrame) + (sizeof(Object*) * stack_size)))
};

#endif
