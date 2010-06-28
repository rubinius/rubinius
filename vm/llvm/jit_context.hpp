#ifndef RBX_LLVM_JIT_CONTEXT
#define RBX_LLVM_JIT_CONTEXT

#include <list>

namespace rubinius {
namespace jit {
  class RuntimeData;
  class RuntimeDataHolder;

  class Context {
    LLVMState* ls_;
    JITMethodInfo* root_info_;
    bool inlined_block_;
    int inline_depth_;

    RuntimeDataHolder* rds_;

  public:
    Context(LLVMState* ls);

    LLVMState* state() {
      return ls_;
    }

    void set_root(JITMethodInfo* info) {
      root_info_ = info;
    }

    JITMethodInfo* root() {
      return root_info_;
    }

    void set_inlined_block(bool val=true) {
      inlined_block_ = val;
    }

    int enter_inline() {
      return ++inline_depth_;
    }

    int leave_inline() {
      return --inline_depth_;
    }

    int inline_depth() {
      return inline_depth_;
    }

    bool inlined_block() {
      return inlined_block_;
    }

    RuntimeDataHolder* runtime_data_holder() {
      return rds_;
    }

    void add_runtime_data(jit::RuntimeData* rd);

    std::ostream& inline_log(const char* header);
  };

}}

#endif
