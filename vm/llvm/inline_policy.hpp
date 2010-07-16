#ifndef RBX_LLVM_INLINE_POLICY_HPP
#define RBX_LLVM_INLINE_POLICY_HPP

#include "vmmethod.hpp"
#include "instructions_util.hpp"

namespace rubinius {
  struct InlineOptions {
    bool check_size;
    bool allow_blocks;
    bool allow_raise_flow;
    bool allow_splat;

    InlineOptions()
      : check_size(true)
      , allow_blocks(true)
      , allow_raise_flow(true)
      , allow_splat(false)
    {}

    void inlining_block() {
      allow_blocks = false;
      allow_raise_flow = false;
      allow_splat = true;
    }
  };

  class InlineEvaluator : public VisitInstructions<InlineEvaluator> {
  public:

    class Unsupported {};

    static bool can_inline_p(VMMethod* vmm, InlineOptions& opts) {
      // Reject methods with splat arguments
      if(!opts.allow_splat && vmm->splat_position >= 0) return false;

      InlineEvaluator eval(opts);

      try {
        eval.drive(vmm);
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

    void visit_push_proc() {
      throw Unsupported();
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

    void visit_yield_splat(opcode count) {
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
    cTooBig
  };

  class InlinePolicy {
  protected:
    VMMethod* vmm_;
    size_t current_size_;
    size_t max_size_;

  public:
    static InlinePolicy* create_policy(VMMethod* vmm);

    InlinePolicy(VMMethod* vmm, int max)
      : vmm_(vmm)
      , current_size_(vmm->total)
      , max_size_(max)
    {}

    virtual ~InlinePolicy() {}

    int current_size() {
      return current_size_;
    }

    int max_size() {
      return max_size_;
    }

    bool check_size_p(VMMethod* other) {
      if(other->total + current_size_ > max_size_) return false;
      return true;
    }

    InlineDecision inline_p(VMMethod* vmm, InlineOptions& opts) {
      if(!InlineEvaluator::can_inline_p(vmm, opts)) {
        return cTooComplex;
      }

      if(opts.check_size && !check_size_p(vmm)) return cTooBig;
      return cInline;
    }

    void increase_size(VMMethod* vmm) {
      current_size_ += vmm->total;
    }
  };

  class SmallMethodInlinePolicy : public InlinePolicy {
  public:
    const static bool is_small_p(VMMethod* vmm) {
      if(vmm->total < 100) return true;
      return false;
    }

    SmallMethodInlinePolicy(VMMethod* vmm)
      : InlinePolicy(vmm, 300) // Random number!
    {}

  };

  class NormalMethodInlinePolicy : public InlinePolicy {
    size_t max_size_;

  public:
    const static bool is_normal_p(VMMethod* vmm) {
      if(vmm->total < 300) return true;
      return false;
    }

    NormalMethodInlinePolicy(VMMethod* vmm)
      : InlinePolicy(vmm, vmm->total * 3)
    {}

  };

  class LargeMethodInlinePolicy : public InlinePolicy {
    size_t max_size_;

  public:
    const static bool is_large_p(VMMethod* vmm) {
      if(vmm->total < 2000) return true;
      return false;
    }

    LargeMethodInlinePolicy(VMMethod* vmm)
      : InlinePolicy(vmm, vmm->total * 2)
    {}
  };

  inline InlinePolicy* InlinePolicy::create_policy(VMMethod* vmm) {
    if(SmallMethodInlinePolicy::is_small_p(vmm)) {
      return new SmallMethodInlinePolicy(vmm);
    } else if(NormalMethodInlinePolicy::is_normal_p(vmm)) {
      return new NormalMethodInlinePolicy(vmm);
    } else if(LargeMethodInlinePolicy::is_large_p(vmm)) {
      return new LargeMethodInlinePolicy(vmm);
    }

    return NULL;
  }
}

#endif
