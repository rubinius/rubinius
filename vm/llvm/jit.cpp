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
      state->shared.llvm_state = new LLVMState(state);
    }

    return state->shared.llvm_state;
  }

  const llvm::Type* LLVMState::ptr_type(std::string name) {
    std::string full_name = std::string("struct.rubinius::") + name;
    return PointerType::getUnqual(
        module_->getTypeByName(full_name.c_str()));
  }

  class BackgroundCompilerThread : public thread::Thread {
    thread::Mutex list_mutex_;
    std::list<BackgroundCompileRequest*> pending_requests_;
    thread::Condition condition_;

    LLVMState* ls_;
    bool show_machine_code_;

  public:
    BackgroundCompilerThread(LLVMState* ls)
      : ls_(ls)
    {
      show_machine_code_ = ls->jit_dump_code() & cMachineCode;
    }

    void add(BackgroundCompileRequest* req) {
      thread::Mutex::LockGuard guard(list_mutex_);
      pending_requests_.push_back(req);
      condition_.signal();
    }

    virtual void perform() {
      for(;;) { // forever

        BackgroundCompileRequest* req = 0;

        // Lock, wait, get a request, unlock
        {
          thread::Mutex::LockGuard guard(list_mutex_);

          // unlock and wait...
          condition_.wait(list_mutex_);
          condition_.reset();

          // now locked again, shift a request
          req = pending_requests_.front();
          pending_requests_.pop_front();
        }

        // mutex now unlock, allowing others to push more requests
        //

        LLVMCompiler* jit = new LLVMCompiler();
        jit->compile(ls_, req->vmmethod());
        jit->generate_function(ls_);

        if(show_machine_code_) {
          jit->show_machine_code();
        }

        // Ok, compiled, generated machine code, now update MachineMethod
        {
          GlobalLock::LockGuard lock(ls_->global_lock());

          MachineMethod* mm = req->machine_method();
          mm->update(req->vmmethod(), jit);
          mm->activate();

          if(ls_->config().jit_show_compiling) {
            std::cout << "[[[ JIT finished background compiling of one method ]]]\n";
          }
        }

        delete req;
      }
    }
  };

  namespace {
    void add_fast_passes(FunctionPassManager* passes) {
      // Eliminate unnecessary alloca.
      passes->add(createPromoteMemoryToRegisterPass());
      // Do simple "peephole" optimizations and bit-twiddling optzns.
      passes->add(createInstructionCombiningPass());
      // Reassociate expressions.
      passes->add(createReassociatePass());
      // Eliminate Common SubExpressions.
      passes->add(createGVNPass());
      passes->add(createDeadStoreEliminationPass());

      passes->add(createInstructionCombiningPass());

      passes->add(createScalarReplAggregatesPass());
      passes->add(createLICMPass());

      passes->add(createSCCPPass());
      passes->add(createInstructionCombiningPass());
      passes->add(createCondPropagationPass());

      // Simplify the control flow graph (deleting unreachable blocks, etc).
      passes->add(createCFGSimplificationPass());

      // passes->add(createGVNPass());
      // passes->add(createCFGSimplificationPass());
      passes->add(createDeadStoreEliminationPass());
      passes->add(createAggressiveDCEPass());
      // passes->add(createVerifierPass());
    }
  }

  LLVMState::LLVMState(STATE)
    : config_(state->shared.config)
    , global_lock_(state->global_lock())
    , symbols_(state->symbols)
  {
    bool fast_code_passes = false;

    module_ = new llvm::Module("rubinius");

    autogen_types::makeLLVMModuleContents(module_);

    mp_ = new llvm::ExistingModuleProvider(module_);
    engine_ = ExecutionEngine::create(mp_);

    passes_ = new llvm::FunctionPassManager(mp_);
    passes_->add(new llvm::TargetData(*engine_->getTargetData()));

    if(fast_code_passes) {
      add_fast_passes(passes_);
    } else {
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
    }

    object_ = ptr_type("Object");

    background_thread_ = new BackgroundCompilerThread(this);
    background_thread_->run();
  }

  Symbol* LLVMState::symbol(const char* sym) {
    return symbols_.lookup(sym);
  }

  void LLVMState::compile_soon(STATE, VMMethod* vmm) {
    MachineMethod* mm = state->new_struct<MachineMethod>(G(machine_method));

    BackgroundCompileRequest* req = new BackgroundCompileRequest(state, vmm, mm);

    background_thread_->add(req);
  }

  static Value* get_field(BasicBlock* block, Value* val, int which) {
    Value* idx[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, which)
    };
    Value* gep = GetElementPtrInst::Create(val, idx, idx+2, "gep", block);
    return gep;
  }

  void LLVMCompiler::import_args(LLVMState* ls,
      Function* func, BasicBlock*& block, VMMethod* vmm,
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

    BasicBlock* cont = BasicBlock::Create("method_body", func);
    BasicBlock* arg_error = BasicBlock::Create("arg_error", func);

    // Check arguments
    //
    // if there is a splat..
    if(vmm->splat_position >= 0) {
      if(vmm->required_args > 0) {
        // Make sure we got at least the required args
        Value* cmp = new ICmpInst(ICmpInst::ICMP_SLT, total,
            ConstantInt::get(Type::Int32Ty, vmm->required_args), "arg_cmp", block);
        BranchInst::Create(arg_error, cont, cmp, block);
      } else {
        // Only splat or optionals, no handling!
        BranchInst::Create(cont, block);
      }

    // No splat, a precise number of args
    } else if(vmm->required_args == vmm->total_args) {
      // Make sure we got the exact number of arguments
      Value* cmp = new ICmpInst(ICmpInst::ICMP_NE, total,
          ConstantInt::get(Type::Int32Ty, vmm->required_args), "arg_cmp", block);
      BranchInst::Create(arg_error, cont, cmp, block);

    // No splat, with optionals
    } else {
      Value* c1 = new ICmpInst(ICmpInst::ICMP_SLT, total,
          ConstantInt::get(Type::Int32Ty, vmm->required_args), "arg_cmp", block);
      Value* c2 = new ICmpInst(ICmpInst::ICMP_SGT, total,
          ConstantInt::get(Type::Int32Ty, vmm->total_args), "arg_cmp", block);

      Value* cmp = BinaryOperator::CreateOr(c1, c2, "arg_combine", block);
      BranchInst::Create(arg_error, cont, cmp, block);
    }

    // Call our arg_error helper
    Signature sig(ls, "Object");

    sig << "VM";
    sig << "CallFrame";
    sig << "Dispatch";
    sig << "Arguments";
    sig << Type::Int32Ty;

    Value* call_args[] = {
      vm_obj,
      call_frame,
      dis_obj,
      arg_obj,
      ConstantInt::get(Type::Int32Ty, vmm->required_args)
    };

    Value* val = sig.call("rbx_arg_error", call_args, 5, "ret", arg_error);
    ReturnInst::Create(val, arg_error);

    // Switch to using continuation
    block = cont;

    // Prepare the scope
    Signature sig2(ls, "Object");

    sig2 << "VM";
    sig2 << "VariableScope";
    sig2 << "CallFrame";
    sig2 << "Dispatch";
    sig2 << "Arguments";

    Value* call_args2[] = {
      vm_obj,
      vars,
      call_frame,
      dis_obj,
      arg_obj
    };

    sig2.call("rbx_setup_scope", call_args2, 5, "", block);

    // Import the arguments
    Value* idx1[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, 3),
    };

    Value* offset = GetElementPtrInst::Create(arg_obj, idx1, idx1+2, "arg_ary_pos", block);

    Value* arg_ary = new LoadInst(offset, "arg_ary", block);

    // If there are a precise number of args, easy.
    if(vmm->required_args == vmm->total_args) {
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

    // Otherwise, we must loop in the generate code because we don't know
    // how many they've actually passed in.
    } else {
      Value* loop_i = new AllocaInst(Type::Int32Ty, 0, "loop_i", block);

      BasicBlock* top = BasicBlock::Create("arg_loop_top", func);
      BasicBlock* body = BasicBlock::Create("arg_loop_body", func);
      BasicBlock* after = BasicBlock::Create("arg_loop_cont", func);

      new StoreInst(ConstantInt::get(Type::Int32Ty, 0), loop_i, false, block);
      BranchInst::Create(top, block);

      // now at the top of block, check if we should continue...
      Value* loop_val = new LoadInst(loop_i, "loop_val", top);
      Value* cmp = new ICmpInst(ICmpInst::ICMP_SLT, loop_val, total,
                                    "loop_test", top);

      BranchInst::Create(body, after, cmp, top);

      // Now, the body

      Value* arg_val_offset =
        GetElementPtrInst::Create(arg_ary, loop_val, "arg_val_offset", body);

      Value* arg_val = new LoadInst(arg_val_offset, "arg_val", body);

      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 8),
        loop_val
      };

      Value* pos = GetElementPtrInst::Create(vars, idx2, idx2+3, "var_pos", body);

      new StoreInst(arg_val, pos, false, body);

      Value* plus_one = BinaryOperator::CreateAdd(loop_val,
          ConstantInt::get(Type::Int32Ty, 1), "add", body);
      new StoreInst(plus_one, loop_i, false, body);

      BranchInst::Create(top, body);

      block = after;
    }

    // Setup the splat.
    if(vmm->splat_position >= 0) {
      Signature sig(ls, "Object");
      sig << "VM";
      sig << "CallFrame";
      sig << "Arguments";
      sig << Type::Int32Ty;

      Value* call_args[] = {
        vm_obj,
        call_frame,
        arg_obj,
        ConstantInt::get(Type::Int32Ty, vmm->total_args)
      };

      Value* splat_val = sig.call("rbx_construct_splat", call_args, 4, "splat_val", block);

      Value* idx3[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 8),
        ConstantInt::get(Type::Int32Ty, vmm->splat_position)
      };

      Value* pos = GetElementPtrInst::Create(vars, idx3, idx3+3, "splat_pos", block);
      new StoreInst(splat_val, pos, false, block);
    }
  }

  void LLVMCompiler::initialize_call_frame(Function* func,
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
    int current_ip_;
    int force_break_;

  public:

    BlockFinder(BlockMap& map, Function* func)
      : map_(map)
      , function_(func)
      , current_ip_(0)
      , force_break_(false)
    {}

    void at_ip(int ip) {
      current_ip_ = ip;
      if(force_break_) {
        break_at(ip);
        force_break_ = false;
      }
    }

    void break_at(int ip) {
      BlockMap::iterator i = map_.find(ip);
      if(i == map_.end()) {
        std::ostringstream ss;
        ss << "ip" << ip;
        map_[ip] = BasicBlock::Create(ss.str().c_str(), function_);
      }
    }

    void next_ip_too() {
      force_break_ = true;
    }

    void visit_goto(opcode which) {
      break_at(which);
      next_ip_too();
    }

    void visit_goto_if_true(opcode which) {
      break_at(which);
    }

    void visit_goto_if_false(opcode which) {
      break_at(which);
    }

    void visit_goto_if_defined(opcode which) {
      break_at(which);
    }

    void visit_setup_unwind(opcode which, opcode type) {
      break_at(which);
    }

    void visit_ret() {
      next_ip_too();
    }

    void visit_raise_return() {
      next_ip_too();
    }

    void visit_raise_break() {
      next_ip_too();
    }

    void visit_ensure_return() {
      next_ip_too();
    }

    void visit_reraise() {
      next_ip_too();
    }

    void visit_raise_exc() {
      next_ip_too();
    }
  };

  BasicBlock* nil_stack(Value* stack, int size, Value* nil,
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

  void LLVMCompiler::compile(LLVMState* ls, VMMethod* vmm) {
    llvm::Module* mod = ls->module();

    const Type* cf_type =
      mod->getTypeByName("struct.rubinius::CallFrame");

    const Type* vars_type =
      mod->getTypeByName("struct.rubinius::VariableScope");

    const Type* obj_type = ls->ptr_type("Object");
    const Type* obj_ary_type = PointerType::getUnqual(obj_type);

    Signature sig(ls, "Object");
    sig << "VM";
    sig << "CallFrame";
    sig << "Dispatch";
    sig << "Arguments";

    Function* func = sig.function("");

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

    initialize_call_frame(func, bb, cf, vmm->stack_size, stk, vars);

    Value* stack_top = new AllocaInst(obj_ary_type, NULL, "stack_top", bb);

    Value* stk_idx[] = {
      ConstantInt::get(Type::Int32Ty, (uint64_t)-1),
    };

    Value* stk_back_one = GetElementPtrInst::Create(stk, stk_idx,
        stk_idx+1, "stk_back_one", bb);
    new StoreInst(stk_back_one, stack_top, false, bb);

    bb = nil_stack(stk, vmm->stack_size, constant(Qnil, obj_type, bb), func, bb);

    import_args(ls, func, bb, vmm, vars, cf);

    JITVisit visitor(ls, vmm, mod, func, bb, stk, cf, vars, stack_top);

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

    if(llvm::verifyFunction(*func, PrintMessageAction)) {
      std::cout << "ERROR: complication error detected.\n";
      std::cout << "ERROR: Please report the above message and the\n";
      std::cout << "       code below to http://github.com/evanphx/rubinius/issues\n";
      std::cout << *func << "\n";
      function_ = NULL;
      return;
    }

    if(ls->jit_dump_code() & cSimple) {
      std::cout << "[[[ LLVM Simple IR ]]]\n";
      std::cout << *func << "\n";
    }

    ls->passes()->run(*func);

    if(ls->jit_dump_code() & cOptimized) {
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

  void* LLVMCompiler::function_pointer() {
    if(!mci_) return NULL;
    return mci_->address();
  }

  void LLVMCompiler::show_machine_code() {
    std::cout << "[[[ JIT Machine Code ]]]\n";
    assembler_x86::AssemblerX86::show_buffer(mci_->address(), mci_->size(), false, NULL);
  }

  void* LLVMCompiler::generate_function(LLVMState* ls) {
    if(!mci_) {
      if(!function_) return NULL;
      mci_ = new llvm::MachineCodeInfo();
      ls->engine()->runJITOnFunction(function_, mci_);
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
