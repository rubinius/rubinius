#include "instructions_util.hpp"

#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/sendsite.hpp"

#include <llvm/DerivedTypes.h>

namespace rubinius {
  class JITVisit : public VisitInstructions<JITVisit> {
    STATE;
    VMMethod* vmm_;

    llvm::Value* stack_;
    llvm::Value* call_frame_;
    llvm::Value* vars_;
    llvm::Module* module_;
    llvm::BasicBlock* block_;
    llvm::Function* function_;

    llvm::Value* stack_top_;

    const llvm::Type* IntPtrTy;
    const llvm::Type* ObjType;
    const llvm::Type* ObjArrayTy;
    const llvm::Type* Int31Ty;

    // Frequently used types
    const llvm::Type* VMTy;
    const llvm::Type* CallFrameTy;

    llvm::Value* vm_;

    bool allow_private_;

    // Cached Function*s
    llvm::Function* rbx_simple_send_;
    llvm::Function* rbx_simple_send_private_;

  public:

    class Unsupported {};

    JITVisit(STATE, VMMethod* vmm,
             llvm::Module* mod, llvm::Function* func, llvm::BasicBlock* block,
             llvm::Value* stack, llvm::Value* call_frame, llvm::Value* vars,
             llvm::Value* stack_top)
      : state(state)
      , vmm_(vmm)
      , stack_(stack)
      , call_frame_(call_frame)
      , vars_(vars)
      , module_(mod)
      , block_(block)
      , function_(func)
      , stack_top_(stack_top)
      , allow_private_(false)
      , rbx_simple_send_(0)
      , rbx_simple_send_private_(0)
    {
#if __LP64__
      IntPtrTy = llvm::Type::Int64Ty;
#else
      IntPtrTy = llvm::Type::Int32Ty;
#endif

      ObjType = PointerType::getUnqual(
          mod->getTypeByName("struct.rubinius::Object"));
      ObjArrayTy = PointerType::getUnqual(ObjType);

      Int31Ty = llvm::IntegerType::get(31);

      VMTy = PointerType::getUnqual(module_->getTypeByName("struct.rubinius::VM"));
      CallFrameTy =
        PointerType::getUnqual(module_->getTypeByName("struct.rubinius::CallFrame"));

      Function::arg_iterator input = function_->arg_begin();
      vm_ = input++;
    }

    Value* stack_ptr(BasicBlock* block = NULL) {
      if(!block) block = block_;
      return new LoadInst(stack_top_, "stack_ptr", block);
    }

    void set_stack_ptr(Value* pos, BasicBlock* block = NULL) {
      if(!block) block = block_;
      new StoreInst(pos, stack_top_, false, block);
    }

    Value* stack_position(int amount, BasicBlock* block = NULL) {
      if(!block) block = block_;

      if(amount == 0) return stack_ptr(block);

      Value* idx = ConstantInt::get(Type::Int32Ty, amount);

      Value* stack_pos = GetElementPtrInst::Create(stack_ptr(block),
                           &idx, &idx+1, "stack_pos", block);

      return stack_pos;
    }

    Value* stack_back_position(int back, BasicBlock* block = NULL) {
      if(!block) block = block_;
      return stack_position(-back, block);
    }

    Value* stack_objects(int count, BasicBlock* block = NULL) {
      if(!block) block = block_;
      return stack_position(-(count - 1), block);
    }

    Value* stack_ptr_adjust(int amount, BasicBlock* block = NULL) {
      if(!block) block = block_;

      Value* pos = stack_position(amount, block);
      set_stack_ptr(pos, block);

      return pos;
    }

    void stack_remove(int count=1) {
      stack_ptr_adjust(-count);
    }

    void stack_push(Value* val, BasicBlock* block = NULL) {
      if(!block) block = block_;
      Value* stack_pos = stack_ptr_adjust(1, block);
      new StoreInst(val, stack_pos, false, block);
    }

    llvm::Value* stack_back(int back, BasicBlock* block = NULL) {
      if(!block) block = block_;
      return new LoadInst(stack_back_position(back, block), "stack_load", block);
    }

    llvm::Value* stack_top() {
      return stack_back(0);
    }

    llvm::Value* stack_pop(BasicBlock* block = NULL) {
      if(!block) block = block_;

      Value* val = stack_back(0, block);

      stack_ptr_adjust(-1, block);
      return val;
    }

    Value* constant(Object* obj, BasicBlock* block = NULL) {
      if(!block) block = block_;
      return CastInst::Create(
          Instruction::IntToPtr,
          ConstantInt::get(IntPtrTy, (intptr_t)obj),
          ObjType, "cast_to_obj", block);
    }

    // visitors.

    void visit(opcode op, opcode arg1, opcode arg2) {
      throw Unsupported();
    }

    void visit_pop() {
      stack_remove(1);
    }

    void visit_push_nil() {
      stack_push(constant(Qnil));
    }

    void visit_push_true() {
      stack_push(constant(Qtrue));
    }

    void visit_push_false() {
      stack_push(constant(Qfalse));
    }

    void visit_push_int(opcode arg) {
      stack_push(constant(Fixnum::from(arg)));
    }

    void visit_meta_push_0() {
      stack_push(constant(Fixnum::from(0)));
    }

    void visit_meta_push_1() {
      stack_push(constant(Fixnum::from(1)));
    }

    void visit_meta_push_2() {
      stack_push(constant(Fixnum::from(2)));
    }

    void visit_meta_push_neg_1() {
      stack_push(constant(Fixnum::from(-1)));
    }

    void visit_ret() {
      ReturnInst::Create(stack_top(), block_);
    }

    void visit_swap_stack() {
      Value* one = stack_pop();
      Value* two = stack_pop();

      stack_push(two);
      stack_push(one);
    }

    void visit_dup_top() {
      stack_push(stack_top());
    }

    Value* cast_int(Value* obj, BasicBlock* block = NULL) {
      if(!block) block = block_;

      return CastInst::Create(
          Instruction::PtrToInt,
          obj,
          IntPtrTy, "cast", block);
    }

    void check_fixnums(Value* left, Value* right, BasicBlock* if_true,
                       BasicBlock* if_false) {
      Value* mask = ConstantInt::get(IntPtrTy, TAG_FIXNUM_MASK);
      Value* tag  = ConstantInt::get(IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(left);
      Value* rint = cast_int(right);
      Value* both =   BinaryOperator::CreateAnd(lint, rint, "both", block_);
      Value* masked = BinaryOperator::CreateAnd(both, mask, "masked", block_);

      Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, masked, tag, "are_fixnums", block_);

      BranchInst::Create(if_true, if_false, cmp, block_);
    }

    void check_both_not_references(Value* left, Value* right, BasicBlock* if_true,
                            BasicBlock* if_false) {
      Value* mask = ConstantInt::get(IntPtrTy, TAG_REF_MASK);
      Value* zero = ConstantInt::get(IntPtrTy, TAG_REF);

      Value* lint = cast_int(left);
      lint = BinaryOperator::CreateAnd(lint, mask, "mask", block_);
      Value* lcmp = new ICmpInst(ICmpInst::ICMP_NE, lint, zero, "check_mask", block_);

      BasicBlock* right_check = BasicBlock::Create("ref_check", function_);
      right_check->moveAfter(block_);
      BranchInst::Create(right_check, if_false, lcmp, block_);

      Value* rint = cast_int(right, right_check);
      rint = BinaryOperator::CreateAnd(rint, mask, "mask", right_check);
      Value* rcmp = new ICmpInst(ICmpInst::ICMP_NE, rint, zero, "check_mask", right_check);

      BranchInst::Create(if_true, if_false, rcmp, right_check);
    }

    void add_send_args(std::vector<const Type*>& types) {
      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(IntPtrTy);
      types.push_back(ObjArrayTy);
    }

    Function* rbx_simple_send() {
      if(rbx_simple_send_) return rbx_simple_send_;

      std::vector<const Type*> types;
      add_send_args(types);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_simple_send", ft));

      rbx_simple_send_ = func;

      return func;
    }

    Function* rbx_simple_send_private() {
      if(rbx_simple_send_private_) return rbx_simple_send_private_;

      std::vector<const Type*> types;
      add_send_args(types);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_simple_send_private", ft));

      rbx_simple_send_private_ = func;

      return func;
    }

    Value* simple_send(Symbol* name, int args, BasicBlock* block=NULL, bool priv=false) {
      if(!block) block = block_;

      Function* func;
      if(priv) {
        func = rbx_simple_send_private();
      } else {
        func = rbx_simple_send();
      }

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(name, block),
        ConstantInt::get(IntPtrTy, args),
        stack_objects(args + 1, block)
      };

      Value* ret = CallInst::Create(func, call_args, call_args+5, "simple_send", block);

      // TODO handle exception
      return ret;
    }

    Value* block_send(Symbol* name, int args, BasicBlock* block=NULL, bool priv=false) {
      if(!block) block = block_;

      std::vector<const Type*> types;
      add_send_args(types);

      char* func_name;
      if(priv) {
        func_name = "rbx_block_send_private";
      } else {
        func_name = "rbx_block_send";
      }

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction(func_name, ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(name, block),
        ConstantInt::get(IntPtrTy, args),
        stack_objects(args + 2, block),   // 2 == recv + block
      };
      Value* ret = CallInst::Create(func, call_args, call_args+5, "block_send", block);

      // TODO handle exception
      return ret;
    }

    Value* splat_send(Symbol* name, int args, BasicBlock* block=NULL, bool priv=false) {
      if(!block) block = block_;

      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(IntPtrTy);
      types.push_back(ObjArrayTy);

      char* func_name;
      if(priv) {
        func_name = "rbx_splat_send_private";
      } else {
        func_name = "rbx_splat_send";
      }

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction(func_name, ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(name, block),
        ConstantInt::get(IntPtrTy, args),
        stack_objects(args + 3, block),   // 3 == recv + block + splat
      };
      Value* ret = CallInst::Create(func, call_args, call_args+5, "splat_send", block);

      // TODO handle exception
      return ret;
    }

    Value* super_send(Symbol* name, int args, BasicBlock* block=NULL, bool splat=false) {
      if(!block) block = block_;

      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(IntPtrTy);
      types.push_back(ObjArrayTy);

      char* func_name;
      int extra = 1;
      if(splat) {
        func_name = "rbx_super_splat_send";
        extra++;
      } else {
        func_name = "rbx_super_send";
      }

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction(func_name, ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(name, block),
        ConstantInt::get(IntPtrTy, args),
        stack_objects(args + extra, block),
      };
      Value* ret = CallInst::Create(func, call_args, call_args+5, "super_send", block);

      // TODO handle exception
      return ret;
    }

    void visit_meta_send_op_equal() {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = BasicBlock::Create("fast", function_);
      BasicBlock* dispatch = BasicBlock::Create("dispatch", function_);
      BasicBlock* cont = BasicBlock::Create("cont", function_);

      check_both_not_references(recv, arg, fast, dispatch);

      Value* called_value = simple_send(state->symbol("=="), 1, dispatch);
      BranchInst::Create(cont, dispatch);

      ICmpInst* cmp = new ICmpInst(ICmpInst::ICMP_EQ,
          recv, arg, "imm_cmp", fast);
      Value* imm_value = SelectInst::Create(cmp, constant(Qtrue, fast),
          constant(Qfalse, fast), "select_bool", fast);

      BranchInst::Create(cont, fast);

      block_ = cont;

      PHINode* phi = PHINode::Create(ObjType, "equal_value", block_);
      phi->addIncoming(called_value, dispatch);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    Value* tag_strip(Value* obj, BasicBlock* block = NULL) {
      if(!block) block = block_;
      Value* i = CastInst::Create(
          Instruction::PtrToInt,
          obj, Int31Ty, "as_int", block);

      return BinaryOperator::CreateLShr(i, ConstantInt::get(Int31Ty, 1), "lshr", block);
    }

    Value* fixnum_tag(Value* obj, BasicBlock* block = NULL) {
      if(!block) block = block_;
      Value* one = ConstantInt::get(Int31Ty, 1);
      Value* more = BinaryOperator::CreateShl(obj, one, "shl", block);
      Value* tagged = BinaryOperator::CreateOr(more, one, "or", block);

      return CastInst::Create(
          Instruction::IntToPtr, tagged, ObjType, "as_obj", block);
    }

    void visit_meta_send_op_plus() {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = BasicBlock::Create("fast", function_);
      BasicBlock* dispatch = BasicBlock::Create("dispatch", function_);
      BasicBlock* cont = BasicBlock::Create("cont", function_);

      check_fixnums(recv, arg, fast, dispatch);

      Value* called_value = simple_send(state->symbol("+"), 1, dispatch);
      BranchInst::Create(cont, dispatch);

      std::vector<const Type*> types;
      types.push_back(Int31Ty);
      types.push_back(Int31Ty);

      std::vector<const Type*> struct_types;
      struct_types.push_back(Int31Ty);
      struct_types.push_back(Type::Int1Ty);

      StructType* st = StructType::get(struct_types);

      FunctionType* ft = FunctionType::get(st, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("llvm.sadd.with.overflow.i31", ft));

      Value* recv_int = tag_strip(recv, fast);
      Value* arg_int = tag_strip(arg, fast);
      Value* call_args[] = { recv_int, arg_int };
      Value* res = CallInst::Create(func, call_args, call_args+2, "add.overflow", fast);

      Value* sum = ExtractValueInst::Create(res, 0, "sum", fast);
      Value* dof = ExtractValueInst::Create(res, 1, "did_overflow", fast);

      BasicBlock* tagnow = BasicBlock::Create("tagnow", function_);
      BranchInst::Create(dispatch, tagnow, dof, fast);

      Value* imm_value = fixnum_tag(sum, tagnow);

      BranchInst::Create(cont, tagnow);

      block_ = cont;

      PHINode* phi = PHINode::Create(ObjType, "addition", block_);
      phi->addIncoming(called_value, dispatch);
      phi->addIncoming(imm_value, tagnow);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_push_literal(opcode which) {
      Object* lit = vmm_->original.get()->literals()->at(state, which);
      if(Symbol* sym = try_as<Symbol>(lit)) {
        stack_push(constant(sym));
      } else {
        Value* idx[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, 3)
        };

        Value* gep = GetElementPtrInst::Create(call_frame_, idx, idx+2, "cm_pos", block_);
        Value* cm =  new LoadInst(gep, "cm", block_);

        idx[1] = ConstantInt::get(Type::Int32Ty, 13);
        gep = GetElementPtrInst::Create(cm, idx, idx+2, "literals_pos", block_);
        Value* lits = new LoadInst(gep, "literals", block_);

        Value* idx2[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, 2),
          ConstantInt::get(Type::Int32Ty, which)
        };

        gep = GetElementPtrInst::Create(lits, idx2, idx2+3, "literal_pos", block_);
        stack_push(new LoadInst(gep, "literal", block_));
      }
    }

    void visit_string_dup() {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_string_dup", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_pop()
      };

      stack_push(CallInst::Create(func, call_args, call_args+3, "string_dup", block_));
    }

    void visit_push_local(opcode which) {
      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 8),
        ConstantInt::get(Type::Int32Ty, which)
      };

      Value* pos = GetElementPtrInst::Create(vars_, idx2, idx2+3, "local_pos", block_);

      stack_push(new LoadInst(pos, "local", block_));
    }

    void visit_set_local(opcode which) {
      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 8),
        ConstantInt::get(Type::Int32Ty, which)
      };

      Value* pos = GetElementPtrInst::Create(vars_, idx2, idx2+3, "local_pos", block_);

      new StoreInst(stack_top(), pos, false, block_);
    }

    void visit_push_self() {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 6)
      };

      Value* pos = GetElementPtrInst::Create(vars_, idx, idx + 2, "self_pos", block_);

      stack_push(new LoadInst(pos, "self", block_));
    }

    void visit_allow_private() {
      allow_private_ = true;
    }

    void visit_send_stack(opcode which, opcode args) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = simple_send(cache->name, args, block_, allow_private_);
      stack_remove(args + 1);
      stack_push(ret);
      allow_private_ = false;
    }

    void visit_send_method(opcode which) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = simple_send(cache->name, 0, block_, allow_private_);
      stack_remove(1);
      stack_push(ret);

      allow_private_ = false;
    }

    void visit_create_block(opcode which) {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(Type::Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_create_block", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, which)
      };

      stack_push(CallInst::Create(func, call_args, call_args+3, "create_block", block_));
    }

    void visit_send_stack_with_block(opcode which, opcode args) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = block_send(cache->name, args, block_, allow_private_);
      stack_remove(args + 2);
      stack_push(ret);
      allow_private_ = false;
    }

    void visit_send_stack_with_splat(opcode which, opcode args) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = splat_send(cache->name, args, block_, allow_private_);
      stack_remove(args + 3);
      stack_push(ret);
      allow_private_ = false;
    }

    void visit_cast_array() {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_cast_array", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_pop()
      };

      stack_push(CallInst::Create(func, call_args, call_args+3, "cast_array", block_));
    }

    void visit_push_block() {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 7)
      };

      Value* gep = GetElementPtrInst::Create(call_frame_, idx, idx+2, "top_scope_pos", block_);
      Value* ts =  new LoadInst(gep, "top_scope", block_);

      idx[1] = ConstantInt::get(Type::Int32Ty, 1);
      gep = GetElementPtrInst::Create(ts, idx, idx+2, "block_pos", block_);
      stack_push(new LoadInst(gep, "block", block_));
    }

    void visit_send_super_stack_with_block(opcode which, opcode args) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = super_send(cache->name, args);
      stack_remove(args + 1);
      stack_push(ret);
    }

    void visit_send_super_stack_with_splat(opcode which, opcode args) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = super_send(cache->name, args, block_, true);
      stack_remove(args + 2);
      stack_push(ret);
    }
  };
}
