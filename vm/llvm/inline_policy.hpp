#ifndef RBX_LLVM_INLINE_POLICY_HPP
#define RBX_LLVM_INLINE_POLICY_HPP

#include "machine_code.hpp"
#include "instructions_util.hpp"
#include "llvm/jit_context.hpp"
#include "vm/util/logger.hpp"

namespace rubinius {
  struct InlineOptions {
    bool check_size;
    bool allow_blocks;
    bool allow_raise_flow;
    bool allow_splat;
    bool allow_exceptions;
    bool allow_has_block;

    InlineOptions()
      : check_size(true)
      , allow_blocks(true)
      , allow_raise_flow(true)
      , allow_splat(false)
      , allow_exceptions(true)
      , allow_has_block(true)
    {}

    void inlining_block() {
      allow_blocks = false;
      allow_raise_flow = false;
      allow_splat = false;
      allow_exceptions = false;
      allow_has_block = false;
    }
  };

  class InlineEvaluator : public VisitInstructions<InlineEvaluator> {
  public:

    static bool can_inline_p(MachineCode* mcode, InlineOptions& opts) {
      // Reject methods with splat arguments
      if(!opts.allow_splat && mcode->splat_position >= 0) return false;

      InlineEvaluator eval(opts);

      try {
        eval.drive(mcode);
      } catch(LLVMState::CompileError& e) {
        utilities::logger::warn("JIT: inline evaluator: compile error: %s", e.error());
        return false;
      }

      return true;
    }

    InlineOptions options_;

    InlineEvaluator(InlineOptions& opts)
      : options_(opts)
    {}

    void visit_push_block() {
      throw LLVMState::CompileError("push_block");
    }

    void visit_push_has_block() {
      if(!options_.allow_has_block) {
        throw LLVMState::CompileError("push_has_block");
      }
    }

    void visit_yield_splat(opcode count) {
      throw LLVMState::CompileError("yield_splat");
    }

    void visit_push_proc() {
      throw LLVMState::CompileError("push_proc");
    }

    void visit_push_block_arg() {
      throw LLVMState::CompileError("push_block_arg");
    }

    void visit_setup_unwind(opcode where, opcode type) {
      if(!options_.allow_exceptions) {
        throw LLVMState::CompileError("setup_unwind");
      }
    }

    void visit_create_block(opcode which) {
      if(!options_.allow_blocks) {
        throw LLVMState::CompileError("create_block");
      }
    }

    void visit_send_super_stack_with_block(opcode which, opcode args) {
      throw LLVMState::CompileError("send_super_stack_with_block");
    }

    void visit_send_super_stack_with_splat(opcode which, opcode args) {
      throw LLVMState::CompileError("send_super_stack_with_splat");
    }

    void visit_zsuper(opcode which) {
      throw LLVMState::CompileError("zsuper");
    }

    void visit_raise_return() {
      if(!options_.allow_raise_flow) {
        throw LLVMState::CompileError("raise_return");
      }
    }

    void visit_raise_break() {
      if(!options_.allow_raise_flow) {
        throw LLVMState::CompileError("raise_break");
      }
    }
  };

  enum InlineDecision {
    cNoPolicy,
    cInline,
    cTooComplex,
    cTooBig,
    cInlineDisabled,
    cTooFewSends
  };

  class InlinePolicy {
  protected:
    MachineCode* machine_code_;
    size_t current_size_;
    size_t max_size_;

  public:
    static InlinePolicy* create_policy(Context* ctx, MachineCode* mcode);

    InlinePolicy(MachineCode* mcode, int max)
      : machine_code_(mcode)
      , current_size_(mcode->total)
      , max_size_(max)
    {}

    virtual ~InlinePolicy() {}

    size_t current_size() {
      return current_size_;
    }

    size_t max_size() {
      return max_size_;
    }

    bool check_size_p(MachineCode* other) {
      if(other->total + current_size_ > max_size_) return false;
      return true;
    }

    InlineDecision inline_p(MachineCode* mcode, InlineOptions& opts) {
      if(!InlineEvaluator::can_inline_p(mcode, opts)) {
        return cTooComplex;
      }

      if(opts.check_size && !check_size_p(mcode)) return cTooBig;
      return cInline;
    }

    void increase_size(MachineCode* mcode) {
      current_size_ += mcode->total;
    }
  };

  class SmallMethodInlinePolicy : public InlinePolicy {
  public:
    static bool is_small_p(Context* ctx, MachineCode* mcode) {
      if(mcode->total < (size_t)ctx->llvm_state()->config().jit_limit_inline_small) return true;
      return false;
    }

    SmallMethodInlinePolicy(Context* ctx, MachineCode* mcode)
      : InlinePolicy(mcode, (size_t)ctx->llvm_state()->config().jit_limit_inline_normal)
    {}

  };

  class NormalMethodInlinePolicy : public InlinePolicy {
  public:
    static bool is_normal_p(Context* ctx, MachineCode* mcode) {
      if(mcode->total < (size_t)ctx->llvm_state()->config().jit_limit_inline_normal) return true;
      return false;
    }

    NormalMethodInlinePolicy(Context* ctx, MachineCode* mcode)
      : InlinePolicy(mcode, mcode->total * 3)
    {}

  };

  class LargeMethodInlinePolicy : public InlinePolicy {
  public:
    static bool is_large_p(Context* ctx, MachineCode* mcode) {
      if(mcode->total < (size_t)ctx->llvm_state()->config().jit_limit_inline_large) return true;
      return false;
    }

    LargeMethodInlinePolicy(Context* ctx, MachineCode* mcode)
      : InlinePolicy(mcode, mcode->total * 2)
    {}
  };

  class NoChangeInlinePolicy : public InlinePolicy {
  public:
    NoChangeInlinePolicy(Context* ctx, MachineCode* mcode)
      : InlinePolicy(mcode, mcode->total)
    {}
  };

  inline InlinePolicy* InlinePolicy::create_policy(Context* ctx, MachineCode* mcode) {
    if(SmallMethodInlinePolicy::is_small_p(ctx, mcode)) {
      return new SmallMethodInlinePolicy(ctx, mcode);
    } else if(NormalMethodInlinePolicy::is_normal_p(ctx, mcode)) {
      return new NormalMethodInlinePolicy(ctx, mcode);
    } else if(LargeMethodInlinePolicy::is_large_p(ctx, mcode)) {
      return new LargeMethodInlinePolicy(ctx, mcode);
    } else {
      return new NoChangeInlinePolicy(ctx, mcode);
    }

    return NULL;
  }
}

#endif
