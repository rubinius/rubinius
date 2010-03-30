#ifdef ENABLE_LLVM

#include "vmmethod.hpp"
#include "llvm/jit.hpp"
#include "llvm/jit_context.hpp"

#include "builtin/fixnum.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"
#include "field_offset.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"

#include "instruments/profiler.hpp"
#include "objectmemory.hpp"

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

      // If we're not in JIT debug mode, delete the body IR, now that we're
      // done with it.
      // This saves us 100M+ of memory in a full spec run.
      if(!ls->debug_p()) {
        function_->dropAllReferences();
      }
    }

    return mci_->address();
  }

  void Compiler::compile_block(LLVMState* ls, CompiledMethod* cm, VMMethod* vmm) {
    if(ls->config().jit_inline_debug) {
      assert(vmm->parent());

      ls->log() << "JIT: compiling block in "
        << ls->symbol_cstr(cm->name())
        << " near "
        << ls->symbol_cstr(cm->file()) << ":"
        << cm->start_line() << "\n";
    }

    jit::Context ctx(ls);
    JITMethodInfo info(ctx, cm, vmm);
    info.is_block = true;

    ctx.set_root(&info);

    jit::BlockBuilder work(ls, info);
    work.setup();

    compile_builder(ctx, ls, info, work);
  }

  void Compiler::compile_method(LLVMState* ls, CompiledMethod* cm, VMMethod* vmm) {
    if(ls->config().jit_inline_debug) {
      ls->log() << "JIT: compiling "
        << ls->symbol_cstr(cm->scope()->module()->name())
        << "#"
        << ls->symbol_cstr(cm->name()) << "\n";
    }

    jit::Context ctx(ls);
    JITMethodInfo info(ctx, cm, vmm);
    info.is_block = false;

    ctx.set_root(&info);

    jit::MethodBuilder work(ls, info);
    work.setup();

    compile_builder(ctx, ls, info, work);
  }

  void Compiler::compile_builder(jit::Context& ctx, LLVMState* ls, JITMethodInfo& info,
                                     jit::Builder& work)
  {
    llvm::Function* func = info.function();

    if(!work.generate_body()) {
      function_ = NULL;
      // This is too noisy to report
      // llvm::outs() << "not supported yet.\n";
      return;
    }

    // Hook up the return pad and return phi.
    work.generate_hard_return();

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
        << ls->symbol_cstr(info.method()->name()) << " ]]]\n";
      llvm::outs() << *func << "\n";
    }

    // Inject the RuntimeData objects used into the original CompiledMethod
    // Do this way after we've validated the IR so things are consistent.
    ctx.runtime_data_holder()->set_function(func);

    info.method()->set_jit_data(ctx.runtime_data_holder());
    ls->shared().om->add_code_resource(ctx.runtime_data_holder());

    function_ = func;

  }
}
}

#endif
