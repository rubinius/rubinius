#ifdef ENABLE_LLVM

#include "llvm/state.hpp"
#include "llvm/jit_context.hpp"
#include "llvm/jit_runtime.hpp"

#include "machine_code.hpp"

using namespace llvm;

namespace rubinius {
namespace jit {

  Context::Context(LLVMState* ls)
    : ls_(ls)
    , root_info_(0)
    , inlined_block_(false)
    , inline_depth_(0)
    , rds_(new RuntimeDataHolder)
    , function_(0)
    , vm_(0)
    , out_args_(0)
    , counter_(0)
    , unwind_info_(0)
  {}

  void Context::init_variables(llvm::IRBuilder<>& b) {
    counter_ = b.CreateAlloca(ls_->Int32Ty, 0, "counter_alloca");

    // The 3 here is because we store {ip, sp, type} per unwind.
    unwind_info_ = b.CreateAlloca(ls_->Int32Ty,
          ConstantInt::get(ls_->Int32Ty, rubinius::kMaxUnwindInfos * 3),
          "unwind_info");

    out_args_ = b.CreateAlloca(ls_->type("Arguments"), 0, "out_args");
  }

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

  void Context::info(const char* msg) {
    if(!ls_->config().jit_inline_debug) return;

    std::ostream& l = ls_->log();

    for(int i = 0; i <= inline_depth_ - 1; i++) {
      l << "|";
    }

    l << "+ " << msg << "\n";
  }

  std::ostream& Context::info_log(const char* header) {
    std::ostream& l = ls_->log();

    for(int i = 0; i <= inline_depth_ - 1; i++) {
      l << "|";
    }

    l << "+ " << header << ": ";
    return l;
  }


}}

#endif
