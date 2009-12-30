#ifdef ENABLE_LLVM

#include "vmmethod.hpp"
#include "llvm/jit.hpp"

#include "builtin/fixnum.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"
#include "field_offset.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"

#include "instruments/profiler.hpp"

#include <llvm/Target/TargetData.h>
// #include <llvm/LinkAllPasses.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/CallingConv.h>
#include <llvm/Support/CFG.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Target/TargetSelect.h>

#include <llvm/Target/TargetOptions.h>

#include <sstream>

#include "llvm/jit.hpp"
#include "llvm/jit_compiler.hpp"
#include "llvm/jit_method.hpp"
#include "llvm/jit_block.hpp"
#include "llvm/passes.hpp"
#include "instructions_util.hpp"

using namespace llvm;

namespace rubinius {
namespace jit {
  void* Compiler::function_pointer() {
    if(!mci_) return NULL;
    return mci_->address();
  }

  void Compiler::show_machine_code() {
    llvm::outs() << "[[[ JIT Machine Code: " << function_->getName() << " ]]]\n";
    LLVMState::show_machine_code(mci_->address(), mci_->size());
  }

  void* Compiler::generate_function(LLVMState* ls) {
    if(!mci_) {
      if(!function_) return NULL;
      mci_ = new llvm::MachineCodeInfo();
      ls->engine()->runJITOnFunction(function_, mci_);
      ls->add_code_bytes(mci_->size());
    }

    return mci_->address();
  }

  void Compiler::compile_block(LLVMState* ls, VMMethod* vmm) {
    if(ls->config().jit_inline_debug) {
      VMMethod* parent = vmm->parent();
      assert(parent);

      ls->log() << "JIT: compiling block in "
        << ls->symbol_cstr(parent->original->scope()->module()->name())
        << "#"
        << ls->symbol_cstr(vmm->original->name())
        << " near "
        << ls->symbol_cstr(vmm->original->file()) << ":"
        << vmm->original->start_line() << "\n";
    }

    JITMethodInfo info(vmm);
    info.is_block = true;

    jit::BlockBuilder work(ls, info);
    work.setup();

    compile_builder(ls, info, work);
  }

  void Compiler::compile_method(LLVMState* ls, VMMethod* vmm) {
    if(ls->config().jit_inline_debug) {
      ls->log() << "JIT: compiling "
        << ls->symbol_cstr(vmm->original->scope()->module()->name())
        << "#"
        << ls->symbol_cstr(vmm->original->name()) << "\n";
    }

    JITMethodInfo info(vmm);
    info.is_block = false;

    jit::MethodBuilder work(ls, info);
    work.setup();

    compile_builder(ls, info, work);
  }

  void Compiler::compile_builder(LLVMState* ls, JITMethodInfo& info,
                                     jit::Builder& work)
  {
    llvm::Function* func = info.function();

    if(!work.generate_body()) {
      function_ = NULL;
      // This is too noisy to report
      // llvm::outs() << "not supported yet.\n";
      return;
    }

    if(ls->jit_dump_code() & cSimple) {
      llvm::outs() << "[[[ LLVM Simple IR ]]]\n";
      llvm::outs() << *func << "\n";
    }

    std::vector<BasicBlock*> to_remove;
    bool Broken = false;
    for(Function::iterator I = func->begin(), E = func->end(); I != E; ++I) {
      if(I->empty()) {
        BasicBlock& bb = *I;
        // No one jumps to it....
        if(llvm::pred_begin(&bb) == llvm::pred_end(&bb)) {
          to_remove.push_back(&bb);
        } else {
          llvm::outs() << "Basic Block is empty and used!\n";
        }
      } else if(!I->back().isTerminator()) {
        llvm::errs() << "Basic Block does not have terminator!\n";
        llvm::errs() << *I << "\n";
        llvm::errs() << "\n";
        Broken = true;
      }
    }

    for(std::vector<BasicBlock*>::iterator i = to_remove.begin();
        i != to_remove.end();
        i++) {
      (*i)->eraseFromParent();
    }

    if(Broken or llvm::verifyFunction(*func, PrintMessageAction)) {
      llvm::outs() << "ERROR: complication error detected.\n";
      llvm::outs() << "ERROR: Please report the above message and the\n";
      llvm::outs() << "       code below to http://github.com/evanphx/rubinius/issues\n";
      llvm::outs() << *func << "\n";
      function_ = NULL;
      return;
    }

    ls->passes()->run(*func);

    if(ls->jit_dump_code() & cOptimized) {
      llvm::outs() << "[[[ LLVM Optimized IR: "
        << ls->symbol_cstr(info.vmm->original->name()) << " ]]]\n";
      llvm::outs() << *func << "\n";
    }

    function_ = func;

  }
}
}

#endif
