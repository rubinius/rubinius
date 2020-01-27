#ifndef RBX_CALL_FRAME_HPP
#define RBX_CALL_FRAME_HPP

#include "stack_variables.hpp"
#include "dispatch.hpp"
#include "arguments.hpp"
#include "object_utils.hpp"

#include "class/compiled_code.hpp"
#include "class/symbol.hpp"

#include <ostream>

namespace rubinius {
  class Module;
  class VariableScope;
  class NativeMethodFrame;
  class BlockEnvironment;
  class MachineCode;
  class UnwindSite;

  namespace jit {
    class RuntimeData;
    class RuntimeDataHolder;
  }

  struct InterpreterState {
    int call_flags;

    InterpreterState()
      : call_flags(0)
    {}
  };

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
    LexicalScope* lexical_scope_;

    void* dispatch_data;
    CompiledCode* compiled_code;

    int flags;
    int ip_;

    VariableScope* top_scope_;
    StackVariables* scope;

    Arguments* arguments;
    Object** stack_ptr_;
    Object** registers;
    MachineCode* machine_code;
    InterpreterState* is;

    UnwindSite* unwind;

    Object* return_value;

    // Stack
    Object* stk[0];

    CallFrame()
      : previous(nullptr)
      , lexical_scope_(nullptr)
      , dispatch_data(nullptr)
      , compiled_code(nullptr)
      , flags(0)
      , ip_(0)
      , top_scope_(nullptr)
      , scope(nullptr)
      , arguments(nullptr)
      , stack_ptr_(nullptr)
      , registers(nullptr)
      , machine_code(nullptr)
      , is(nullptr)
      , unwind(nullptr)
      , return_value(nullptr)
    {
    }

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

    Symbol* name() const {
      if(inline_method_p() && dispatch_data) {
        // TODO: JIT
        return nil<Symbol>();
      } else if(block_p()) {
        return nil<Symbol>();
      } else if(arguments) {
        return arguments->name();
      }

      return nil<Symbol>();
    }

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

    LexicalScope* lexical_scope() const {
      return lexical_scope_;
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

    void jit_fixup(STATE, CallFrame* creator);

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

    void next_ip(int width) {
      ip_ += width;
    }

    void push_unwind(UnwindSite* unwind);
    UnwindSite* pop_unwind();

    /**
     *  Initialize frame for the given stack size.
     */
    void prepare(int stack) {
      ip_ = 0;
      return_value = nullptr;

      for(int i = 0; i < stack; i++) {
        stk[i] = cNil;
      }

      registers = stk + stack;
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

    void dump();

    Object* find_breakpoint(STATE);
  };

#define ALLOCA_CALL_FRAME(stack_size) \
  static_cast<uintptr_t*>(alloca(sizeof(CallFrame) + (sizeof(Object*) * (stack_size))))
};

#endif
