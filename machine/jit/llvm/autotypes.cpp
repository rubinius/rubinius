#ifdef ENABLE_LLVM

#include "config.h"
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/DataLayout.h>
#elif RBX_LLVM_API_VER >= 302
#include <llvm/DataLayout.h>
#else
#include <llvm/Target/TargetData.h>
#endif
#if RBX_LLVM_API_VER >= 305
#include <llvm/IR/Verifier.h>
#include <llvm/IR/CFG.h>
#else
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/CFG.h>
#endif
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Module.h>
#else
#include <llvm/CallingConv.h>
#include <llvm/Module.h>
#endif
#include <llvm/Transforms/Scalar.h>
#include <llvm/Analysis/Passes.h>
// #include <llvm/LinkAllPasses.h>
#include <llvm/Target/TargetOptions.h>

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
#ifdef IS_64BIT_ARCH
#include "jit/llvm/types64.cpp.gen"
#else
#include "jit/llvm/types32.cpp.gen"
#endif
}

#endif
