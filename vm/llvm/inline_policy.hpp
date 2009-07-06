#ifndef RBX_LLVM_INLINE_POLICY_HPP
#define RBX_LLVM_INLINE_POLICY_HPP

#include "vmmethod.hpp"
#include "instructions_util.hpp"

namespace rubinius {
  class InlineEvaluator : public VisitInstructions<InlineEvaluator> {
  public:

    class Unsupported {};

    static bool can_inline_p(VMMethod* vmm) {
      // Reject methods with 'complex' arguments
      if(vmm->splat_position >= 0 ||
          vmm->required_args != vmm->total_args) return false;

      InlineEvaluator eval;

      try {
        eval.drive(vmm);
      } catch(Unsupported& e) {
        return false;
      }

      return true;
    }

    void visit_create_block(opcode which) {
      throw Unsupported();
    }

    void visit_setup_unwind(opcode which, opcode type) {
      throw Unsupported();
    }

    void visit_send_super_stack_with_block(opcode which, opcode args) {
      throw Unsupported();
    }

    void visit_send_super_stack_with_splat(opcode which, opcode args) {
      throw Unsupported();
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

    InlineDecision inline_p(VMMethod* vmm) {
      if(!InlineEvaluator::can_inline_p(vmm)) return cTooComplex;
      if(!check_size_p(vmm)) return cTooBig;

      current_size_ += vmm->total;
      return cInline;
    }
  };

  class SmallMethodInlinePolicy : public InlinePolicy {
  public:
    const static bool is_small_p(VMMethod* vmm) {
      if(vmm->total < 10) return true;
      return false;
    }

    SmallMethodInlinePolicy(VMMethod* vmm)
      : InlinePolicy(vmm, 200) // Random number!
    {}

  };

  class NormalMethodInlinePolicy : public InlinePolicy {
    size_t max_size_;

  public:
    const static bool is_normal_p(VMMethod* vmm) {
      if(vmm->total < 200) return true;
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
