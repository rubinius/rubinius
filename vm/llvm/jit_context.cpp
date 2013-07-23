#ifdef ENABLE_LLVM

#include "llvm/state.hpp"
#include "llvm/jit_context.hpp"
#include "llvm/jit_runtime.hpp"
#include "llvm/jit_memory_manager.hpp"
#include "llvm/passes.hpp"
#include "llvm/jit_builder.hpp"

#include "machine_code.hpp"

#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/DataLayout.h>
#elif RBX_LLVM_API_VER >= 302
#include <llvm/DataLayout.h>
#else
#include <llvm/Target/TargetData.h>
#endif
#include <llvm/Transforms/Scalar.h>

using namespace llvm;

namespace autogen_types {
  void makeLLVMModuleContents(llvm::Module* module);
}

namespace rubinius {

  void IRBuilderInserterWithDebug::InsertHelper(llvm::Instruction *I, const llvm::Twine &Name,
                    llvm::BasicBlock *BB, llvm::BasicBlock::iterator InsertPt) const {
    I->setDebugLoc(builder_->b().getCurrentDebugLocation());
    if(BB) BB->getInstList().insert(InsertPt, I);
    I->setName(Name);
  }

  Context::Context(LLVMState* ls)
    : ls_(ls)
    , root_info_(0)
    , inlined_block_(false)
    , inline_depth_(0)
    , rds_(new jit::RuntimeDataHolder)
    , function_(0)
    , state_(0)
    , out_args_(0)
    , counter_(0)
  {
    VoidTy = Type::getVoidTy(ctx_);

    Int1Ty = Type::getInt1Ty(ctx_);
    Int8Ty = Type::getInt8Ty(ctx_);
    Int16Ty = Type::getInt16Ty(ctx_);
    Int32Ty = Type::getInt32Ty(ctx_);
    Int64Ty = Type::getInt64Ty(ctx_);

#ifdef IS_X8664
    IntPtrTy = Int64Ty;
#else
    IntPtrTy = Int32Ty;
#endif

    VoidPtrTy = llvm::PointerType::getUnqual(Int8Ty);

    FloatTy = Type::getFloatTy(ctx_);
    DoubleTy = Type::getDoubleTy(ctx_);

    Int8PtrTy = llvm::PointerType::getUnqual(Int8Ty);

    Zero = llvm::ConstantInt::get(Int32Ty, 0);
    One = llvm::ConstantInt::get(Int32Ty, 1);

    module_ = new llvm::Module("rubinius", ctx_);

    autogen_types::makeLLVMModuleContents(module_);

    llvm::EngineBuilder factory(module_);
    std::string error;

    factory.setAllocateGVsWithCode(false);
    memory_ = new jit::RubiniusRequestJITMemoryManager(ls->memory());
    factory.setJITMemoryManager(memory_);
    factory.setEngineKind(EngineKind::JIT);
    factory.setErrorStr(&error);

#if RBX_LLVM_API_VER > 300
    llvm::TargetOptions opts;
    opts.NoFramePointerElim = true;
    opts.NoFramePointerElimNonLeaf = true;
    opts.JITEmitDebugInfo = true;

    factory.setTargetOptions(opts);
#endif

    factory.setMCPU(ls_->cpu());

    engine_ = factory.create();
    if(!engine_) {
      std::cerr << "Error setting up LLVM Execution Engine: "<< error << std::endl;
      rubinius::bug("error configuring LLVM");
    }
    if(ls_->jit_event_listener()) {
      engine_->RegisterJITEventListener(ls_->jit_event_listener());
    }

    builder_ = new llvm::PassManagerBuilder();
    builder_->OptLevel = 2;
    passes_ = new llvm::FunctionPassManager(module_);

#if RBX_LLVM_API_VER >= 302
    module_->setDataLayout(engine_->getDataLayout()->getStringRepresentation());
    passes_->add(new llvm::DataLayout(*engine_->getDataLayout()));
#else
    module_->setDataLayout(engine_->getTargetData()->getStringRepresentation());
    passes_->add(new llvm::TargetData(*engine_->getTargetData()));
#endif

    builder_->populateFunctionPassManager(*passes_);

    // Eliminate unnecessary alloca.
    passes_->add(createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    passes_->add(createInstructionCombiningPass());
    // Reassociate expressions.
    passes_->add(createReassociatePass());
    // Eliminate Common SubExpressions.
    passes_->add(createGVNPass());
    passes_->add(createDeadStoreEliminationPass());

    passes_->add(createInstructionCombiningPass());

    // Simplify the control flow graph (deleting unreachable blocks, etc).
    passes_->add(createCFGSimplificationPass());

    passes_->add(create_rubinius_alias_analysis());
    passes_->add(createGVNPass());
    // passes_->add(createCFGSimplificationPass());
    passes_->add(createDeadStoreEliminationPass());
    // passes_->add(createVerifierPass());
    passes_->add(createScalarReplAggregatesPass());

    passes_->add(create_overflow_folding_pass());
    passes_->add(create_guard_eliminator_pass());

    passes_->add(createCFGSimplificationPass());
    passes_->add(createInstructionCombiningPass());
    passes_->add(createScalarReplAggregatesPass());
    passes_->add(createDeadStoreEliminationPass());
    passes_->add(createCFGSimplificationPass());
    passes_->add(createInstructionCombiningPass());
    passes_->doInitialization();

    ObjTy = ptr_type("Object");

    profiling_ = new GlobalVariable(
        *module_, Int1Ty, false,
        GlobalVariable::ExternalLinkage,
        0, "profiling_flag");

    metadata_id_ = ctx_.getMDKindID("rbx-classid");
  }

  Context::~Context() {
    delete builder_;
    delete passes_;
    delete engine_;
    // Memory is cleaned up by the engine
    memory_ = NULL;
  }

  void* Context::native_function() {
    void* addr = memory_->generatedFunction();
    memory_->resetGeneratedFunction();

    engine_->freeMachineCodeForFunction(function_);

    // Nuke the Function from the module
    function_->replaceAllUsesWith(UndefValue::get(function_->getType()));
    function_->removeFromParent();
    return addr;
  }

  llvm::Type* Context::ptr_type(llvm::Type* type) {
    return llvm::PointerType::getUnqual(type);
  }

  llvm::Type* Context::ptr_type(std::string name) {
    std::string full_name = std::string("struct.rubinius::") + name;
    return llvm::PointerType::getUnqual(
        module_->getTypeByName(full_name));
  }

  llvm::Type* Context::type(std::string name) {
    std::string full_name = std::string("struct.rubinius::") + name;
    return module_->getTypeByName(full_name);
  }

  void Context::init_variables(IRBuilder& b) {
    counter_ = b.CreateAlloca(Int32Ty, 0, "counter_alloca");
    out_args_ = b.CreateAlloca(type("Arguments"), 0, "out_args");
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


}

#endif
