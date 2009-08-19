#ifndef RBX_LLVM_PASSES_HPP
#define RBX_LLVM_PASSES_HPP

#include <llvm/Pass.h>

namespace rubinius {
  llvm::FunctionPass* create_overflow_folding_pass();
  llvm::FunctionPass* create_rubinius_alias_analysis();
  llvm::FunctionPass* create_guard_eliminator_pass();
  llvm::FunctionPass* create_allocation_eliminator_pass();
}

#endif
