
#include "vmmethod.hpp"
#include "llvm/jit.hpp"

#include "builtin/fixnum.hpp"
#include "field_offset.hpp"

#include "call_frame.hpp"

#include <llvm/Target/TargetData.h>
// #include <llvm/LinkAllPasses.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>

using namespace llvm;

#include "llvm/jit_visit.hpp"

namespace autogen_types {
#include "gen/types.cpp"
}

namespace rubinius {

  LLVMState* LLVMState::get(STATE) {
    if(!state->shared.llvm_state) {
      state->shared.llvm_state = new LLVMState();
    }

    return state->shared.llvm_state;
  }

  LLVMState::LLVMState() {
    module_ = new llvm::Module("rubinius");

    autogen_types::makeLLVMModuleContents(module_);

    mp_ = new llvm::ExistingModuleProvider(module_);
    engine_ = ExecutionEngine::create(mp_);

    passes_ = new llvm::FunctionPassManager(mp_);
    passes_->add(new llvm::TargetData(*engine_->getTargetData()));

    // while debugging, verify the input.
    passes_->add(createVerifierPass());

    // Eliminate unnecessary alloca.
    passes_->add(createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    passes_->add(createInstructionCombiningPass());
    // Reassociate expressions.
    passes_->add(createReassociatePass());
    // Eliminate Common SubExpressions.
    passes_->add(createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    passes_->add(createCFGSimplificationPass());

    passes_->add(createVerifierPass());

    passes_->add(createInstructionCombiningPass());

    passes_->add(createDeadStoreEliminationPass());
  }

  void import_args(STATE, Function* func, BasicBlock* block, VMMethod* vmm, Value* vars) {
    Function::arg_iterator args = func->arg_begin();
    args++;
    args++;
    args++;
    Value* arg_obj = args++;

    if(vmm->required_args == 0) return;

    Value* idx1[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, 3),
    };

    Value* offset = GetElementPtrInst::Create(arg_obj, idx1, idx1+2, "arg_ary_pos", block);

    Value* arg_ary = new LoadInst(offset, "arg_ary", block);

    for(int i = 0; i < vmm->required_args; i++) {
      Value* int_pos = ConstantInt::get(Type::Int32Ty, i);

      Value* arg_val_offset =
        GetElementPtrInst::Create(arg_ary, int_pos, "arg_val_offset", block);

      Value* arg_val = new LoadInst(arg_val_offset, "arg_val", block);

      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 8),
        int_pos
      };

      Value* pos = GetElementPtrInst::Create(vars, idx2, idx2+3, "var_pos", block);

      new StoreInst(arg_val, pos, false, block);
    }
  }

  void LLVMCompiler::compile(STATE, VMMethod* vmm) {
    llvm::Module* mod = LLVMState::get(state)->module();

    const Type* cf_type =
      mod->getTypeByName("struct.rubinius::CallFrame");

    const Type* vars_type =
      mod->getTypeByName("struct.rubinius::VariableScope");

    std::vector<const Type*> types;

    types.push_back(PointerType::getUnqual(
          mod->getTypeByName("struct.rubinius::VM")));
    types.push_back(PointerType::getUnqual(cf_type));
    types.push_back(PointerType::getUnqual(
          mod->getTypeByName("struct.rubinius::Dispatch")));
    types.push_back(PointerType::getUnqual(
          mod->getTypeByName("struct.rubinius::Arguments")));

    const Type* obj_type = PointerType::getUnqual(
        mod->getTypeByName("struct.rubinius::Object"));

    FunctionType* ft = FunctionType::get(obj_type, types, false);
    Function* func = cast<Function>(mod->getOrInsertFunction("", ft));

    Function::arg_iterator ai = func->arg_begin();
    (ai++)->setName("state");
    (ai++)->setName("call_frame");
    (ai++)->setName("msg");
    (ai++)->setName("args");

    BasicBlock* bb = BasicBlock::Create("entry", func);

    Value* cf =  new AllocaInst(cf_type, 0, "cf_alloca", bb);
    Value* stk = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty, vmm->stack_size),
        "stk_alloca", bb);

    Value* var_mem = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty,
          (sizeof(VariableScope) / sizeof(Object*)) + vmm->number_of_locals),
        "var_mem", bb);

    Value* vars = CastInst::Create(
        Instruction::BitCast,
        var_mem,
        PointerType::getUnqual(vars_type), "vars_alloca", bb);

    /*
    Value* js_idx[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, 10),
      ConstantInt::get(Type::Int32Ty, 0)
    };

    Value* js_stack_pos = GetElementPtrInst::Create(cf, js_idx, js_idx+3,
        "jst_stack_pos", block_);

    new StoreInst(stk, js_stack_pos, false, block_);
    */

    import_args(state, func, bb, vmm, vars);

    JITVisit visitor(state, vmm, mod, func, bb, stk, cf, vars);

    try {
      visitor.drive(vmm->opcodes, vmm->total);
    } catch(JITVisit::Unsupported &e) {
      function_ = NULL;
      std::cout << "not supported yet.\n";
      return;
    }

    /*
    (void)cf; (void)stk; (void)vars;

    Value* nil_int = ConstantInt::get(Type::Int32Ty, (intptr_t)Qnil);

    Value* ret = CastInst::Create(
          Instruction::IntToPtr,
          nil_int,
          obj_type, "cast_to_obj", bb);

    Value* pos = ConstantInt::get(Type::Int32Ty, 0);
    Value* stack_0 = GetElementPtrInst::Create(stk, pos, "", bb);
    new StoreInst(ret, stack_0, false, bb);

    Value* pos2 = ConstantInt::get(Type::Int32Ty, 0);
    Value* stack_02 = GetElementPtrInst::Create(stk, pos2, "", bb);
    Value* out =  new LoadInst(stack_02, "load", bb);
    ReturnInst::Create(out, bb);

    */

    std::cout << *func << "\n";

    LLVMState::get(state)->passes()->run(*func);

    function_ = func;
  }

  void* LLVMCompiler::function_pointer(STATE) {
    if(!jit_entry_) {
      if(function_) {
        jit_entry_ = LLVMState::get(state)->engine()->getPointerToFunction(function_);
      }
    }

    return jit_entry_;
  }

  llvm::Function* LLVMCompiler::llvm_function(STATE) {
    return function_;
  }

  void LLVMCompiler::show_assembly(STATE, llvm::Function* func) {
    if(func) {
      std::cout << *func << "\n";
    } else {
      std::cout << "NULL function!\n";
    }
  }

}
