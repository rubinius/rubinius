#ifdef ENABLE_LLVM

#include "vm/config.h"
#if RBX_LLVM_API_VER >= 302
#include <llvm/DataLayout.h>
#else
#include <llvm/Target/TargetData.h>
#endif
// #include <llvm/LinkAllPasses.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/CallingConv.h>
#include <llvm/Support/CFG.h>
#include <llvm/Analysis/Passes.h>

#include <llvm/Target/TargetOptions.h>
#include <llvm/Module.h>

using namespace llvm;

namespace autogen_types {
#include "llvm/types.cpp.gen"
}

#endif
