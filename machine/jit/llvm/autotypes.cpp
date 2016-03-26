#include "config.h"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Module.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Target/TargetOptions.h>

using namespace llvm;

namespace autogen_types {
#ifdef IS_64BIT_ARCH
#include "jit/llvm/types64.cpp.gen"
#else
#include "jit/llvm/types32.cpp.gen"
#endif
}
