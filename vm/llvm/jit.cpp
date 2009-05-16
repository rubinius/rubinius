#ifdef ENABLE_LLVM

#include "vmmethod.hpp"
#include "llvm/jit.hpp"

#include "builtin/fixnum.hpp"
#include "field_offset.hpp"

#include "call_frame.hpp"
#include "assembler/jit.hpp"
#include "configuration.hpp"

#include <llvm/Target/TargetData.h>
// #include <llvm/LinkAllPasses.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>

#include <sstream>

using namespace llvm;

#include "llvm/jit_visit.hpp"

namespace autogen_types {
#include "llvm/types.cpp.gen"
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
    passes_->add(createDeadStoreEliminationPass());

    passes_->add(createInstructionCombiningPass());

    // Simplify the control flow graph (deleting unreachable blocks, etc).
    passes_->add(createCFGSimplificationPass());

    // passes_->add(createGVNPass());
    // passes_->add(createCFGSimplificationPass());
    passes_->add(createDeadStoreEliminationPass());
    // passes_->add(createVerifierPass());

    passes_->add(createVerifierPass());

    object_ = PointerType::getUnqual(
        module_->getTypeByName("struct.rubinius::Object"));
  }

  static Value* get_field(BasicBlock* block, Value* val, int which) {
    Value* idx[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, which)
    };
    Value* gep = GetElementPtrInst::Create(val, idx, idx+2, "gep", block);
    return gep;
  }


  void import_args(STATE, Function* func, BasicBlock*& block, VMMethod* vmm,
                   Value* vars, Value* call_frame) {
    Function::arg_iterator args = func->arg_begin();
    Value* vm_obj = args++;
    args++;
    Value* dis_obj = args++;
    Value* arg_obj = args++;

    // Check the argument count
    Value* total_idx[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, 2),
    };

    Value* total_offset = GetElementPtrInst::Create(arg_obj, total_idx,
        total_idx + 2, "total_pos", block);
    Value* total = new LoadInst(total_offset, "arg.total", block);

    Value* cmp = new ICmpInst(ICmpInst::ICMP_SLT, total,
        ConstantInt::get(Type::Int32Ty, vmm->required_args), "arg_cmp", block);

    BasicBlock* arg_error = BasicBlock::Create("arg_error", func);
    BasicBlock* cont = BasicBlock::Create("method_body", func);

    BranchInst::Create(arg_error, cont, cmp, block);

    // Call our arg_error helper
    std::vector<const Type*> types;

    LLVMState* ls = LLVMState::get(state);
    llvm::Module* module = ls->module();

    types.push_back(
          PointerType::getUnqual(module->getTypeByName("struct.rubinius::VM")));
    types.push_back(
          PointerType::getUnqual(module->getTypeByName("struct.rubinius::CallFrame")));
    types.push_back(
          PointerType::getUnqual(module->getTypeByName("struct.rubinius::Dispatch")));
    types.push_back(
          PointerType::getUnqual(module->getTypeByName("struct.rubinius::Arguments")));
    types.push_back(Type::Int32Ty);

    FunctionType* ft = FunctionType::get(ls->object(), types, false);
    Function* func_ae = cast<Function>(
          module->getOrInsertFunction("rbx_arg_error", ft));

    Value* call_args[] = {
      vm_obj,
      call_frame,
      dis_obj,
      arg_obj,
      ConstantInt::get(Type::Int32Ty, vmm->required_args)
    };

    Value* val = CallInst::Create(func_ae, call_args, call_args+5, "ret", arg_error);
    ReturnInst::Create(val, arg_error);

    // Switch to using continuation
    block = cont;

    // Prepare the scope
    types.clear();
    types.push_back(
          PointerType::getUnqual(module->getTypeByName("struct.rubinius::VM")));
    types.push_back(
          PointerType::getUnqual(module->getTypeByName("struct.rubinius::VariableScope")));
    types.push_back(
          PointerType::getUnqual(module->getTypeByName("struct.rubinius::CallFrame")));
    types.push_back(
          PointerType::getUnqual(module->getTypeByName("struct.rubinius::Dispatch")));
    types.push_back(
          PointerType::getUnqual(module->getTypeByName("struct.rubinius::Arguments")));

    FunctionType* ft2 = FunctionType::get(ls->object(), types, false);
    Function* func_setup = cast<Function>(
          module->getOrInsertFunction("rbx_setup_scope", ft2));

    Value* call_args2[] = {
      vm_obj,
      vars,
      call_frame,
      dis_obj,
      arg_obj
    };

    CallInst::Create(func_setup, call_args2, call_args2+5, "setup", block);

    // Import the arguments
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

  void LLVMCompiler::initialize_call_frame(STATE, Function* func,
      BasicBlock* block, Value* call_frame,
      int stack_size, Value* stack, Value* vars) {

    Function::arg_iterator ai = func->arg_begin();
    ai++; // state
    Value* prev = ai++;
    Value* msg =  ai++;
    Value* args = ai++;

    Value* exec = new LoadInst(get_field(block, msg, 2), "msg.exec", block);
    Value* cm_gep = get_field(block, call_frame, 3);
    Value* meth = CastInst::Create(
        Instruction::BitCast,
        exec,
        cast<PointerType>(cm_gep->getType())->getElementType(),
        "cm", block);

    // previous
    new StoreInst(prev, get_field(block, call_frame, 0), false, block);

    // static_scope
    Value* ss = new LoadInst(get_field(block, meth, 12), "cm.scope", block);
    new StoreInst(ss, get_field(block, call_frame, 1), false, block);

    // name
    Value* name = new LoadInst(get_field(block, msg, 0), "msg.name", block);
    new StoreInst(name, get_field(block, call_frame, 2), false, block);

    // cm
    new StoreInst(meth, cm_gep, false, block);

    // flags
    new StoreInst(ConstantInt::get(Type::Int32Ty, 0),
                  get_field(block, call_frame, 4), false, block);

    // args
    Value* total = new LoadInst(get_field(block, args, 2), "args.total", block);
    new StoreInst(total, get_field(block, call_frame, 5), false, block);

    // ip
    new StoreInst(ConstantInt::get(Type::Int32Ty, 0),
                  get_field(block, call_frame, 6), false, block);

    // top_scope
    new StoreInst(vars, get_field(block, call_frame, 7), false, block);

    // scope
    new StoreInst(vars, get_field(block, call_frame, 8), false, block);

    // stack_size
    new StoreInst(ConstantInt::get(Type::Int32Ty, stack_size),
                  get_field(block, call_frame, 9), false, block);

    // stk
    new StoreInst(stack, get_field(block, call_frame, 10), false, block);

  }

  class BlockFinder : public VisitInstructions<BlockFinder> {
    BlockMap& map_;
    Function* function_;

  public:

    BlockFinder(BlockMap& map, Function* func)
      : map_(map)
      , function_(func)
    {}

    void visit_goto(opcode which) {
      BlockMap::iterator i = map_.find(which);
      if(i == map_.end()) {
        std::ostringstream ss;
        ss << "ip" << which;
        map_[which] = BasicBlock::Create(ss.str().c_str(), function_);
      }
    }

    void visit_goto_if_true(opcode which) {
      visit_goto(which);
    }

    void visit_goto_if_false(opcode which) {
      visit_goto(which);
    }

    void visit_goto_if_defined(opcode which) {
      visit_goto(which);
    }

    void visit_setup_unwind(opcode which, opcode type) {
      visit_goto(which);
    }
  };

  BasicBlock* nil_stack(STATE, Value* stack, int size, Value* nil,
                        Function* function, BasicBlock* block) {
    if(size == 0) return block;
    // Stack size 5 or less, do 5 stores in a row rather than
    // the loop.
    if(size <= 5) {
      for(int i = 0; i < size; i++) {
        Value* idx[] = {
          ConstantInt::get(Type::Int32Ty, i)
        };

        Value* gep = GetElementPtrInst::Create(stack, idx, idx+1, "stack_pos", block);
        new StoreInst(nil, gep, block);
      }
      return block;
    }

    Value* max = ConstantInt::get(Type::Int32Ty, size);
    Value* one = ConstantInt::get(Type::Int32Ty, 1);


    BasicBlock* top = BasicBlock::Create("stack_nil", function);
    BasicBlock* cont = BasicBlock::Create("bottom", function);

    Value* counter = new AllocaInst(Type::Int32Ty, 0, "counter_alloca", block);
    new StoreInst(ConstantInt::get(Type::Int32Ty, 0), counter, block);

    BranchInst::Create(top, block);

    Value* cur = new LoadInst(counter, "counter", top);
    Value* idx[] = { cur };

    Value* gep = GetElementPtrInst::Create(stack, idx, idx+1, "stack_pos", top);
    new StoreInst(nil, gep, top);

    Value* added = BinaryOperator::CreateAdd(cur, one, "added", top);
    new StoreInst(added, counter, top);

    Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, added, max, "loop_check", top);
    BranchInst::Create(cont, top, cmp, top);

    return cont;
  }

  Value* constant(Object* obj, const Type* obj_type, BasicBlock* block) {
    return CastInst::Create(
        Instruction::IntToPtr,
        ConstantInt::get(Type::Int32Ty, (intptr_t)obj),
        obj_type, "cast_to_obj", block);
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

    const Type* obj_ary_type = PointerType::getUnqual(obj_type);

    FunctionType* ft = FunctionType::get(obj_type, types, false);
    Function* func = cast<Function>(mod->getOrInsertFunction("", ft));

    Function::arg_iterator ai = func->arg_begin();
    (ai++)->setName("state");
    (ai++)->setName("previous");
    (ai++)->setName("msg");
    (ai++)->setName("args");

    BasicBlock* bb = BasicBlock::Create("entry", func);

    Value* cf =  new AllocaInst(cf_type, 0, "call_frame", bb);
    Value* stk = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty, vmm->stack_size),
        "stack", bb);

    Value* var_mem = new AllocaInst(obj_type,
        ConstantInt::get(Type::Int32Ty,
          (sizeof(VariableScope) / sizeof(Object*)) + vmm->number_of_locals),
        "var_mem", bb);

    Value* vars = CastInst::Create(
        Instruction::BitCast,
        var_mem,
        PointerType::getUnqual(vars_type), "vars", bb);

    initialize_call_frame(state, func, bb, cf, vmm->stack_size, stk, vars);

    Value* stack_top = new AllocaInst(obj_ary_type, NULL, "stack_top", bb);

    Value* stk_idx[] = {
      ConstantInt::get(Type::Int32Ty, (uint64_t)-1),
    };

    Value* stk_back_one = GetElementPtrInst::Create(stk, stk_idx,
        stk_idx+1, "stk_back_one", bb);
    new StoreInst(stk_back_one, stack_top, false, bb);

    bb = nil_stack(state, stk, vmm->stack_size, constant(Qnil, obj_type, bb), func, bb);

    import_args(state, func, bb, vmm, vars, cf);

    JITVisit visitor(state, vmm, mod, func, bb, stk, cf, vars, stack_top);

    // Pass 1, detect BasicBlock boundaries
    BlockFinder finder(visitor.block_map(), func);
    finder.drive(vmm);

    // Check for interrupts at the top of every method...
    visitor.visit_check_interrupts();

    // Pass 2, compile!
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

    if(state->shared.config.jit_dump_code & cSimple) {
      std::cout << "[[[ LLVM Simple IR ]]]\n";
      std::cout << *func << "\n";
    }

    LLVMState::get(state)->passes()->run(*func);

    if(state->shared.config.jit_dump_code & cOptimized) {
      std::cout << "[[[ LLVM Optimized IR ]]]\n";
      std::cout << *func << "\n";
    }

    function_ = func;
  }

  void* LLVMCompiler::function_pointer(STATE) {
    if(!mci_) {
      if(!function_) return NULL;
      mci_ = new llvm::MachineCodeInfo();
      LLVMState::get(state)->engine()->runJITOnFunction(function_, mci_);

      if(state->shared.config.jit_dump_code & cMachineCode) {
        std::cout << "[[[ JIT Machine Code ]]]\n";
        assembler_x86::AssemblerX86::show_buffer(mci_->address(), mci_->size(), false, NULL);
      }
    }

    return mci_->address();
  }

  llvm::Function* LLVMCompiler::llvm_function(STATE) {
    return function_;
  }

  void LLVMCompiler::show_assembly(STATE) {
    if(function_) {
      std::cout << *function_ << "\n";
      std::cout << "\n== x86 assembly ==\n";

      // Force it to be compiled
      function_pointer(state);
      assembler_x86::AssemblerX86::show_buffer(mci_->address(), mci_->size(), false, NULL);
    } else {
      std::cout << "NULL function!\n";
    }
  }

}

#endif
