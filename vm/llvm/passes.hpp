#ifndef RBX_LLVM_PASSES_HPP
#define RBX_LLVM_PASSES_HPP

#include <llvm/Pass.h>

namespace rubinius {
  llvm::FunctionPass* create_overflow_folding_pass();
}

#endif
