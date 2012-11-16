#ifndef RBX_LLVM_INLINE_POLICY_HPP
#define RBX_LLVM_INLINE_POLICY_HPP

#include "machine_code.hpp"
#include "instructions_util.hpp"
#include "llvm/state.hpp"

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
      allow_splat = true;
      allow_exceptions = false;
      allow_has_block = false;
    }

    void inlining_block_19() {
      allow_blocks = false;
      allow_raise_flow = false;
      allow_splat = false;
      allow_exceptions = false;
      allow_has_block = false;
    }
  };

  class InlineEvaluator : public VisitInstructions<InlineEvaluator> {
  public:

    class Unsupported {};

    static bool can_inline_p(MachineCode* mcode, InlineOptions& opts) {
      // Reject methods with splat arguments
      if(!opts.allow_splat && mcode->splat_position >= 0) return false;
      if(mcode->post_args > 0) return false;

      InlineEvaluator eval(opts);

      try {
        eval.drive(mcode);
      } catch(Unsupported& e) {
        return false;
      }

      return true;
    }

    InlineOptions options_;

    InlineEvaluator(InlineOptions& opts)
      : options_(opts)
    {}

    void visit_push_block() {
      throw Unsupported();
    }

    void visit_push_has_block() {
      if(!options_.allow_has_block) throw Unsupported();
    }

    void visit_yield_splat(opcode count) {
      throw Unsupported();
    }

    void visit_push_proc() {
      throw Unsupported();
    }

    void visit_setup_unwind(opcode where, opcode type) {
      if(!options_.allow_exceptions) throw Unsupported();
    }

    void visit_create_block(opcode which) {
      if(!options_.allow_blocks) throw Unsupported();
    }

    void visit_send_super_stack_with_block(opcode which, opcode args) {
      throw Unsupported();
    }

    void visit_send_super_stack_with_splat(opcode which, opcode args) {
      throw Unsupported();
    }

    void visit_zsuper(opcode which) {
      throw Unsupported();
    }

    void visit_raise_return() {
      if(!options_.allow_raise_flow) throw Unsupported();
    }

    void visit_raise_break() {
      if(!options_.allow_raise_flow) throw Unsupported();
    }
  };

  enum InlineDecision {
    cNoPolicy,
    cInline,
    cTooComplex,
    cTooBig,
    cInlineDisabled
  };

  class InlinePolicy {
  protected:
    MachineCode* machine_code_;
    size_t current_size_;
    size_t max_size_;

  public:
    static InlinePolicy* create_policy(LLVMState* state, MachineCode* mcode);

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
    static bool is_small_p(LLVMState* state, MachineCode* mcode) {
      if(mcode->total < (size_t)state->config().jit_inline_small_method_size) return true;
      return false;
    }

    SmallMethodInlinePolicy(LLVMState* state, MachineCode* mcode)
      : InlinePolicy(mcode, (size_t)state->config().jit_inline_normal_method_size)
    {}

  };

  class NormalMethodInlinePolicy : public InlinePolicy {
  public:
    static bool is_normal_p(LLVMState* state, MachineCode* mcode) {
      if(mcode->total < (size_t)state->config().jit_inline_normal_method_size) return true;
      return false;
    }

    NormalMethodInlinePolicy(LLVMState* state, MachineCode* mcode)
      : InlinePolicy(mcode, mcode->total * 3)
    {}

  };

  class LargeMethodInlinePolicy : public InlinePolicy {
  public:
    static bool is_large_p(LLVMState* state, MachineCode* mcode) {
      if(mcode->total < (size_t)state->config().jit_inline_large_method_size) return true;
      return false;
    }

    LargeMethodInlinePolicy(LLVMState* state, MachineCode* mcode)
      : InlinePolicy(mcode, mcode->total * 2)
    {}
  };

  class NoChangeInlinePolicy : public InlinePolicy {
  public:
    NoChangeInlinePolicy(LLVMState* state, MachineCode* mcode)
      : InlinePolicy(mcode, mcode->total)
    {}
  };

  inline InlinePolicy* InlinePolicy::create_policy(LLVMState* state, MachineCode* mcode) {
    if(SmallMethodInlinePolicy::is_small_p(state, mcode)) {
      return new SmallMethodInlinePolicy(state, mcode);
    } else if(NormalMethodInlinePolicy::is_normal_p(state, mcode)) {
      return new NormalMethodInlinePolicy(state, mcode);
    } else if(LargeMethodInlinePolicy::is_large_p(state, mcode)) {
      return new LargeMethodInlinePolicy(state, mcode);
    } else {
      return new NoChangeInlinePolicy(state, mcode);
    }

    return NULL;
  }
}

#endif
