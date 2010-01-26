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
    , rds_(new RuntimeDataHolder)
  {}

  void Context::add_runtime_data(jit::RuntimeData* rd) {
    rds_->add_runtime_data(rd);
  }

}}

#endif
