#ifdef ENABLE_LLVM

#include "config.h"
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/DataLayout.h>
#elif RBX_LLVM_API_VER >= 302
#include <llvm/DataLayout.h>
#else
#include <llvm/Target/TargetData.h>
#endif
// #include <llvm/LinkAllPasses.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/CallingConv.h>
#else
#include <llvm/CallingConv.h>
#endif
#include <llvm/Support/CFG.h>
#include <llvm/Analysis/Passes.h>

#include <llvm/Target/TargetOptions.h>
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/Module.h>
#else
#include <llvm/Module.h>
#endif

using namespace llvm;

#if RBX_LLVM_API_VER >= 303
// Starting from LLVM 3.3, AttrListPtr is renamed to AttributeSet, which
// affects auto-generated types{32,64}.cpp.gen.
// Alias AttrListPtr to AttributeSet as a temporary work-around at the moment.
namespace llvm {
  typedef AttributeSet AttrListPtr;
};
#endif

namespace autogen_types {
#ifdef IS_X8664
#include "llvm/types64.cpp.gen"
#else
#include "llvm/types32.cpp.gen"
#endif
}

#endif
