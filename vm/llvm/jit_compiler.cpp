#ifdef ENABLE_LLVM

#include "machine_code.hpp"
#include "llvm/jit_context.hpp"

#include "builtin/fixnum.hpp"
#include "builtin/constant_scope.hpp"
#include "builtin/module.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/jit.hpp"
#include "field_offset.hpp"

#include "call_frame.hpp"
#include "configuration.hpp"

#include "object_memory.hpp"

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
#else
#include <llvm/CallingConv.h>
#endif
#include <llvm/Transforms/Scalar.h>
#include <llvm/Analysis/Passes.h>

#include <llvm/Target/TargetOptions.h>

#include <sstream>

#include <sys/time.h>

#include "llvm/state.hpp"
#include "llvm/jit_compiler.hpp"
#include "llvm/jit_method.hpp"
#include "llvm/jit_block.hpp"

#include "llvm/method_info.hpp"

#include "llvm/passes.hpp"
#include "instructions_util.hpp"
#include "dtrace/dtrace.h"

using namespace llvm;

namespace rubinius {
namespace jit {
  void Compiler::show_machine_code() {
    llvm::outs() << "[[[ JIT Machine Code: " << function_->getName() << " ]]]\n";
    LLVMState::show_machine_code(mci_->address(), mci_->size());
  }

  void* Compiler::generate_function(bool indy) {
    if(!mci_) {
      if(!function_) return NULL;

      if(indy) ctx_->llvm_state()->shared().gc_independent(ctx_->llvm_state()->vm());
      if(ctx_->llvm_state()->jit_dump_code() & cSimple) {
        llvm::outs() << "[[[ LLVM Simple IR: " << function_->getName() << " ]]]\n";
        llvm::outs() << *function_ << "\n";
      }

      std::vector<BasicBlock*> to_remove;
      bool Broken = false;
      for(Function::iterator I = function_->begin(),
          E = function_->end();
          I != E;
          ++I)
      {
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
          ++i) {
        (*i)->eraseFromParent();
      }

#if RBX_LLVM_API_VER >= 305
      if(Broken or llvm::verifyFunction(*function_, &llvm::outs())) {
#else
      if(Broken or llvm::verifyFunction(*function_, PrintMessageAction)) {
#endif
        llvm::outs() << "ERROR: compilation error detected.\n";
        llvm::outs() << "ERROR: Please report the above message and the\n";
        llvm::outs() << "       code below to http://github.com/rubinius/rubinius/issues\n";
        llvm::outs() << *function_ << "\n";
        function_ = NULL;
        if(indy) ctx_->llvm_state()->shared().gc_dependent(ctx_->llvm_state()->vm());
        return NULL;
      }

      ctx_->passes()->run(*function_);

      if(ctx_->llvm_state()->jit_dump_code() & cOptimized) {
        llvm::outs() << "[[[ LLVM Optimized IR: " << function_->getName() << " ]]]\n";
        llvm::outs() << *function_ << "\n";
      }

      mci_ = new llvm::MachineCodeInfo();
      ctx_->engine()->runJITOnFunction(function_, mci_);

      // If we're not in JIT debug mode, delete the body IR, now that we're
      // done with it.
      // This saves us 100M+ of memory in a full spec run.
      if(!ctx_->llvm_state()->debug_p()) {
        function_->dropAllReferences();
      }

      if(indy) ctx_->llvm_state()->shared().gc_dependent(ctx_->llvm_state()->vm());

      ctx_->llvm_state()->add_code_bytes(mci_->size());
      // Inject the RuntimeData objects used into the original CompiledCode
      // Do this way after we've validated the IR so things are consistent.

      void* native_function = ctx_->native_function();

      ctx_->runtime_data_holder()->set_function(native_function,
                                   mci_->address(), mci_->size());

      // info.method()->set_jit_data(ctx.runtime_data_holder());
      ctx_->llvm_state()->shared().om->add_code_resource(ctx_->runtime_data_holder());
    }

    return mci_->address();
  }

  void Compiler::compile(JITCompileRequest* req) {
    if(req->is_block()) {
      compile_block(req);
    } else {
      compile_method(req);
    }
  }

  void Compiler::compile_block(JITCompileRequest* req) {

    CompiledCode* code = req->method();
    MachineCode* mcode = req->machine_code();

    if(ctx_->llvm_state()->config().jit_inline_debug) {

      struct timeval tv;
      gettimeofday(&tv, NULL);

      ctx_->llvm_state()->log() << "JIT: compiling block in "
        << ctx_->llvm_state()->symbol_debug_str(code->name())
        << " near "
        << ctx_->llvm_state()->symbol_debug_str(code->file()) << ":"
        << code->start_line()
        << " (" << tv.tv_sec << "." << tv.tv_usec << ")\n";
    }

#ifdef HAVE_DTRACE
    if(RUBINIUS_JIT_FUNCTION_BEGIN_ENABLED()) {
      RBX_DTRACE_CONST char* class_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->enclosure_name(code).c_str());
      RBX_DTRACE_CONST char* method_name =
          const_cast<RBX_DTRACE_CONST char*>("<block>");
      RBX_DTRACE_CONST char* file_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->symbol_debug_str(code->file()).c_str());
      int line = code->start_line();
      RUBINIUS_JIT_FUNCTION_BEGIN(class_name, method_name, file_name, line);
    }
#endif

    JITMethodInfo info(ctx_, code, mcode);
    info.is_block = true;
    info.hits = req->hits();

    if(Class* cls = req->receiver_class()) {
      info.set_self_class(cls);
    }

    ctx_->set_root(&info);

    jit::BlockBuilder work(ctx_, info);
    work.setup();

    compile_builder(info, work);
    ctx_->set_root(NULL);

#ifdef HAVE_DTRACE
    if(RUBINIUS_JIT_FUNCTION_END_ENABLED()) {
      RBX_DTRACE_CONST char* class_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->enclosure_name(code).c_str());
      RBX_DTRACE_CONST char* method_name =
          const_cast<RBX_DTRACE_CONST char*>("<block>");
      RBX_DTRACE_CONST char* file_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->symbol_debug_str(code->file()).c_str());
      int line = code->start_line();
      RUBINIUS_JIT_FUNCTION_END(class_name, method_name, file_name, line);
    }
#endif
  }

  void Compiler::compile_method(JITCompileRequest* req) {
    CompiledCode* code = req->method();

    if(ctx_->llvm_state()->config().jit_inline_debug) {
      struct timeval tv;
      gettimeofday(&tv, NULL);

      ctx_->llvm_state()->log() << "JIT: compiling "
        << ctx_->llvm_state()->enclosure_name(code)
        << "#"
        << ctx_->llvm_state()->symbol_debug_str(code->name())
        << " (" << tv.tv_sec << "." << tv.tv_usec << ")\n";
    }

#ifdef HAVE_DTRACE
    if(RUBINIUS_JIT_FUNCTION_BEGIN_ENABLED()) {
      RBX_DTRACE_CONST char* class_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->enclosure_name(code).c_str());
      RBX_DTRACE_CONST char* method_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->symbol_debug_str(code->name()).c_str());
      RBX_DTRACE_CONST char* file_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->symbol_debug_str(code->file()).c_str());
      int line = code->start_line();
      RUBINIUS_JIT_FUNCTION_BEGIN(class_name, method_name, file_name, line);
    }
#endif

    JITMethodInfo info(ctx_, code, code->machine_code());
    info.is_block = false;
    info.hits = req->hits();

    if(Class* cls = req->receiver_class()) {
      info.set_self_class(cls);
    }

    ctx_->set_root(&info);

    jit::MethodBuilder work(ctx_, info);
    work.setup();

    compile_builder(info, work);
    ctx_->set_root(NULL);

#ifdef HAVE_DTRACE
    if(RUBINIUS_JIT_FUNCTION_END_ENABLED()) {
      RBX_DTRACE_CONST char* class_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->enclosure_name(code).c_str());
      RBX_DTRACE_CONST char* method_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->symbol_debug_str(code->name()).c_str());
      RBX_DTRACE_CONST char* file_name =
          const_cast<RBX_DTRACE_CONST char*>(
              ctx_->llvm_state()->symbol_debug_str(code->file()).c_str());
      int line = code->start_line();
      RUBINIUS_JIT_FUNCTION_END(class_name, method_name, file_name, line);
    }
#endif
  }

  void Compiler::compile_builder(JITMethodInfo& info,
                                 jit::Builder& work)
  {
    function_ = info.function();

    if(!work.generate_body()) {
      ctx_->set_failure();
      function_ = NULL;
      // This is too noisy to report
      // llvm::outs() << "not supported yet.\n";
      return;
    }

    // Hook up the return pad and return phi.
    work.generate_hard_return();
  }
}
}

#endif
