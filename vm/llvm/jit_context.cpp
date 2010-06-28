#ifdef ENABLE_LLVM

#include "llvm/jit.hpp"
#include "llvm/jit_context.hpp"
#include "llvm/jit_runtime.hpp"

namespace rubinius {
namespace jit {

  Context::Context(LLVMState* ls)
    : ls_(ls)
    , root_info_(0)
    , inlined_block_(false)
    , inline_depth_(0)
    , rds_(new RuntimeDataHolder)
  {}

  void Context::add_runtime_data(jit::RuntimeData* rd) {
    rds_->add_runtime_data(rd);
  }

  std::ostream& Context::inline_log(const char* header) {
    std::ostream& l = ls_->log();

    for(int i = 0; i <= inline_depth_; i++) {
      l << "|";
    }

    l << " " << header << ": ";
    return l;
  }

}}

#endif
