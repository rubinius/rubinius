#include "instructions_util.hpp"

#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/sendsite.hpp"

#include <llvm/DerivedTypes.h>

#include <list>

namespace rubinius {
  typedef std::map<int, llvm::BasicBlock*> BlockMap;

  class ExceptionHandler {
    BasicBlock* code_;

  public:
    ExceptionHandler(BasicBlock* code)
      : code_(code)
    {}

    BasicBlock* code() {
      return code_;
    }
  };

  typedef std::list<ExceptionHandler*> EHandlers;

  class JITFunction : public Signature {
    llvm::Function* func_;

  public:
    JITFunction(LLVMState* ls)
      : Signature(ls, Type::VoidTy)
      , func_(0)
    {}

    void resolve(const char* name, const Type* rt) {
      ret_type_ = rt;
      func_ = function(name);
    }

    Value* call(Value** start, int size, const char* inst_name, BasicBlock* block) {
      return llvm::CallInst::Create(func_, start, start+size, inst_name, block);
    }
  };

  class JITFunctions {
  public:
    JITFunction return_to_here;
    JITFunction clear_raise_value;

    JITFunctions(LLVMState* ls)
      : return_to_here(ls)
      , clear_raise_value(ls)
    {
      return_to_here
        << "VM"
        << "CallFrame";

      return_to_here.resolve("rbx_return_to_here", Type::Int1Ty);

      clear_raise_value
        << "VM";

      clear_raise_value.resolve("rbx_clear_raise_value", ls->object());
    }
  };

  class JITVisit : public VisitInstructions<JITVisit> {
    JITFunctions f;
    VMMethod* vmm_;
    BlockMap block_map_;

    llvm::Value* stack_;
    llvm::Value* call_frame_;
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
    opcode call_flags_;

    // Cached Function*s
    llvm::Function* rbx_simple_send_;
    llvm::Function* rbx_simple_send_private_;

    // bail out destinations
    llvm::BasicBlock* bail_out_;
    llvm::BasicBlock* bail_out_fast_;

    EHandlers exception_handlers_;

    LLVMState* ls_;

    Value* method_entry_;
    Value* args_;

    Value* ip_pos_;

  public:

    const llvm::Type* ptr_type(std::string name) {
      std::string full_name = std::string("struct.rubinius::") + name;
      return PointerType::getUnqual(
          module_->getTypeByName(full_name.c_str()));
    }

    class Unsupported {};

    JITVisit(LLVMState* ls, VMMethod* vmm,
             llvm::Module* mod, llvm::Function* func, llvm::BasicBlock* start,
             llvm::Value* stack, llvm::Value* call_frame,
             llvm::Value* stack_top, llvm::Value* me, llvm::Value* args)
      : f(ls)
      , vmm_(vmm)
      , stack_(stack)
      , call_frame_(call_frame)
      , module_(mod)
      , block_(start)
      , function_(func)
      , stack_top_(stack_top)
      , allow_private_(false)
      , call_flags_(0)
      , rbx_simple_send_(0)
      , rbx_simple_send_private_(0)
      , ls_(ls)
      , method_entry_(me)
      , args_(args)
    {
#if __LP64__
      IntPtrTy = llvm::Type::Int64Ty;
#else
      IntPtrTy = llvm::Type::Int32Ty;
#endif

      ObjType = ptr_type("Object");
      ObjArrayTy = PointerType::getUnqual(ObjType);

      Int31Ty = llvm::IntegerType::get(31);

      VMTy = ptr_type("VM");
      CallFrameTy = ptr_type("CallFrame");

      Function::arg_iterator input = function_->arg_begin();
      vm_ = input++;

      bail_out_ = block("bail_out");

      Value* call_args[] = {
        vm_,
        call_frame_
      };

      Value* isit = f.return_to_here.call(call_args, 2, "rth", bail_out_);

      BasicBlock* ret_raise_val = block("ret_raise_val");
      bail_out_fast_ = block("ret_null");

      BranchInst::Create(ret_raise_val, bail_out_fast_, isit, bail_out_);

      ReturnInst::Create(Constant::getNullValue(ObjType), bail_out_fast_);

      Value* crv = f.clear_raise_value.call(&vm_, 1, "crv", ret_raise_val);
      ReturnInst::Create(crv, ret_raise_val);

      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::cf_ip)
      };


      ip_pos_ = GetElementPtrInst::Create(call_frame_, idx, idx+2, "ip_pos", block_);
    }

    Value* scope() {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::cf_scope)
      };

      Value* gep = GetElementPtrInst::Create(call_frame_, idx, idx+2, "scope_pos", block_);
      return new LoadInst(gep, "scope", block_);
    }

    Value* top_scope() {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::cf_top_scope)
      };

      Value* gep = GetElementPtrInst::Create(call_frame_, idx, idx+2, "top_scope_pos", block_);
      return new LoadInst(gep, "top_scope", block_);
    }

    BasicBlock* block(const char* name = "continue") {
      return BasicBlock::Create(name, function_);
    }

    BlockMap& block_map() {
      return block_map_;
    }

    void check_for_return(Value* val) {
      BasicBlock* cont = block();
      BasicBlock* push_val = block("push_val");

      Value* null = Constant::getNullValue(ObjType);

      Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, val, null, "null_check", block_);
      BasicBlock* is_break = block("is_break");
      BranchInst::Create(is_break, push_val, cmp, block_);

      /////
      Signature brk(ls_, Type::Int1Ty);
      brk << VMTy;
      brk << CallFrameTy;

      Value* call_args[] = {
        vm_,
        call_frame_
      };

      Value* isit = brk.call("rbx_break_to_here", call_args, 2, "bth", is_break);

      BasicBlock* push_break_val = block("push_break_val");
      BasicBlock* next = 0;

      // If there are handlers...
      if(exception_handlers_.size() > 0) {
        ExceptionHandler* handler = exception_handlers_.back();
        next = handler->code();
      } else {
        next = bail_out_;
      }

      BranchInst::Create(push_break_val, next, isit, is_break);

      ////
      Signature clear(ls_, ObjType);
      clear << VMTy;
      Value* crv = clear.call("rbx_clear_raise_value", &vm_, 1, "crv", push_break_val);

      stack_push(crv, push_break_val);

      BranchInst::Create(cont, push_break_val);

      /////
      stack_push(val, push_val);
      BranchInst::Create(cont, push_val);

      /////
      block_ = cont;
    }

    void check_for_exception_then(Value* val, BasicBlock* cont, BasicBlock* block) {
      Value* null = Constant::getNullValue(ObjType);

      Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, val, null, "null_check", block);

      // If there are handlers...
      if(exception_handlers_.size() > 0) {
        ExceptionHandler* handler = exception_handlers_.back();
        BranchInst::Create(handler->code(), cont, cmp, block);
      } else {
        BranchInst::Create(bail_out_fast_, cont, cmp, block);
      }
    }

    BasicBlock* check_for_exception(Value* val, BasicBlock* els) {
      BasicBlock* cont = block();
      check_for_exception_then(val, cont, els);

      return cont;
    }

    void check_for_exception(Value* val) {
      block_ = check_for_exception(val, block_);
    }

    void write_barrier(Value* obj, Value* val) {
      Signature wb(ls_, ObjType);
      wb << VMTy;
      wb << ObjType;
      wb << ObjType;

      if(obj->getType() != ObjType) {
        obj = CastInst::Create(
          Instruction::BitCast,
          obj,
          ObjType, "casted", block_);
      }

      Value* call_args[] = {
        vm_,
        obj,
        val
      };

      wb.call("rbx_write_barrier", call_args, 3, "", block_);
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
      if(val->getType() == cast<PointerType>(stack_pos->getType())->getElementType()) {
        new StoreInst(val, stack_pos, false, block);
      } else {
        Value* cst = CastInst::Create(
          Instruction::BitCast,
          val,
          ObjType, "casted", block);
        new StoreInst(cst, stack_pos, false, block);
      }
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

    void at_ip(int ip) {
      BlockMap::iterator i = block_map_.find(ip);
      if(i != block_map_.end()) {
        BasicBlock* next = i->second;
        if(!block_->getTerminator()) {
          BranchInst::Create(next, block_);
        }

        next->moveAfter(block_);

        block_ = next;
      }

      new StoreInst(ConstantInt::get(Type::Int32Ty, ip), ip_pos_, false, block_);
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
      if(ls_->include_profiling()) {
        Value* test = new LoadInst(ls_->profiling(), "profiling", block_);
        BasicBlock* end_profiling = BasicBlock::Create("end_profiling", function_);
        BasicBlock* cont = BasicBlock::Create("continue", function_);

        BranchInst::Create(end_profiling, cont, test, block_);

        block_ = end_profiling;

        Signature sig(ls_, Type::VoidTy);
        sig << PointerType::getUnqual(Type::Int8Ty);

        Value* call_args[] = {
          method_entry_
        };

        sig.call("rbx_end_profiling", call_args, 1, "", block_);

        BranchInst::Create(cont, block_);

        block_ = cont;
      }

      ReturnInst::Create(stack_top(), block_);
    }

    void visit_swap_stack() {
      Value* top = stack_pop();
      Value* bottom = stack_pop();

      stack_push(top);
      stack_push(bottom);
    }

    void visit_dup_top() {
      stack_push(stack_top());
    }

    void visit_rotate(opcode count) {
      int diff = count / 2;

      for(int i = 0; i < diff; i++) {
        int offset = count - i - 1;
        Value* pos = stack_back_position(offset);
        Value* pos2 = stack_back_position(i);

        Value* val = new LoadInst(pos, "rotate", block_);
        Value* val2 = new LoadInst(pos2, "rotate", block_);

        new StoreInst(val2, pos, false, block_);
        new StoreInst(val, pos2, false, block_);
      }
    }

    void visit_move_down(opcode positions) {
      Value* val = stack_top();

      for(opcode i = 0; i < positions; i++) {
        int target = i;
        int current = target + 1;

        Value* tmp = stack_back(current);
        Value* pos = stack_back_position(target);

        new StoreInst(tmp, pos, false, block_);
      }


      new StoreInst(val, stack_back_position(positions), false, block_);
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

      BasicBlock* right_check = block("ref_check");
      right_check->moveAfter(block_);
      BranchInst::Create(right_check, if_false, lcmp, block_);

      Value* rint = cast_int(right, right_check);
      rint = BinaryOperator::CreateAnd(rint, mask, "mask", right_check);
      Value* rcmp = new ICmpInst(ICmpInst::ICMP_NE, rint, zero, "check_mask", right_check);

      BranchInst::Create(if_true, if_false, rcmp, right_check);
    }

    void add_send_args(Signature& sig) {
      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << IntPtrTy;
      sig << ObjArrayTy;
    }

    Function* rbx_simple_send() {
      if(rbx_simple_send_) return rbx_simple_send_;

      Signature sig(ls_, ObjType);
      add_send_args(sig);

      rbx_simple_send_ = sig.function("rbx_simple_send");

      return rbx_simple_send_;
    }

    Function* rbx_simple_send_private() {
      if(rbx_simple_send_private_) return rbx_simple_send_private_;

      Signature sig(ls_, ObjType);
      add_send_args(sig);

      rbx_simple_send_private_ = sig.function("rbx_simple_send");

      return rbx_simple_send_private_;
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

      return CallInst::Create(func, call_args, call_args+5, "simple_send", block);
    }


    Value* inline_cache_send(Symbol* name, int args, SendSite::Internal* cache,
                       BasicBlock* block=NULL, bool priv=false) {
      if(!block) block = block_;

      Signature sig(ls_, ObjType);
      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << "SendSite::Internal";
      sig << IntPtrTy;
      sig << ObjArrayTy;

      const char* func_name;
      if(priv) {
        func_name = "rbx_inline_cache_send_private";
      } else {
        func_name = "rbx_inline_cache_send";
      }

      Value* cache_const = CastInst::Create(
          Instruction::IntToPtr,
          ConstantInt::get(IntPtrTy, (intptr_t)cache),
          ptr_type("SendSite::Internal"), "cast_to_ptr", block);

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(name, block),
        cache_const,
        ConstantInt::get(IntPtrTy, args),
        stack_objects(args + 1, block)
      };

      return sig.call(func_name, call_args, 6, "ic_send", block);
    }

    Value* block_send(Symbol* name, int args, BasicBlock* block=NULL, bool priv=false) {
      if(!block) block = block_;

      Signature sig(ls_, ObjType);
      add_send_args(sig);

      const char* func_name;
      if(priv) {
        func_name = "rbx_block_send_private";
      } else {
        func_name = "rbx_block_send";
      }

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(name, block),
        ConstantInt::get(IntPtrTy, args),
        stack_objects(args + 2, block),   // 2 == recv + block
      };

      return sig.call(func_name, call_args, 5, "block_send", block);
    }

    Value* splat_send(Symbol* name, int args, BasicBlock* block=NULL, bool priv=false) {
      if(!block) block = block_;

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << IntPtrTy;
      sig << ObjArrayTy;

      const char* func_name;
      if(priv) {
        func_name = "rbx_splat_send_private";
      } else {
        func_name = "rbx_splat_send";
      }

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(name, block),
        ConstantInt::get(IntPtrTy, args),
        stack_objects(args + 3, block),   // 3 == recv + block + splat
      };

      return sig.call(func_name, call_args, 5, "splat_send", block);
    }

    Value* super_send(Symbol* name, int args, BasicBlock* block=NULL, bool splat=false) {
      if(!block) block = block_;

      Signature sig(ls_, ObjType);
      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << IntPtrTy;
      sig << ObjArrayTy;

      const char* func_name;
      int extra = 1;
      if(splat) {
        func_name = "rbx_super_splat_send";
        extra++;
      } else {
        func_name = "rbx_super_send";
      }

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(name, block),
        ConstantInt::get(IntPtrTy, args),
        stack_objects(args + extra, block),
      };

      return sig.call(func_name, call_args, 5, "super_send", block);
    }

    void visit_meta_send_op_equal() {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = block("fast");
      BasicBlock* dispatch = block("dispatch");
      BasicBlock* cont = block();

      check_both_not_references(recv, arg, fast, dispatch);

      Value* called_value = simple_send(ls_->symbol("=="), 1, dispatch);
      check_for_exception_then(called_value, cont, dispatch);

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

    void visit_meta_send_op_tequal() {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = block("fast");
      BasicBlock* dispatch = block("dispatch");
      BasicBlock* cont = block();

      check_fixnums(recv, arg, fast, dispatch);

      Value* called_value = simple_send(ls_->symbol("==="), 1, dispatch);
      check_for_exception_then(called_value, cont, dispatch);

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

    Value* tag_strip(Value* obj, BasicBlock* block = NULL, const Type* type = NULL) {
      if(!block) block = block_;
      if(!type) type = Int31Ty;

      Value* i = CastInst::Create(
          Instruction::PtrToInt,
          obj, Type::Int32Ty, "as_int", block);

      Value* more = BinaryOperator::CreateLShr(
          i, ConstantInt::get(Type::Int32Ty, 1),
          "lshr", block);
      return CastInst::CreateIntegerCast(
          more, type, true, "stripped", block);
    }

    Value* fixnum_tag(Value* obj, BasicBlock* block = NULL) {
      if(!block) block = block_;
      Value* obj32 = CastInst::CreateZExtOrBitCast(
          obj, Type::Int32Ty, "as_32bit", block);
      Value* one = ConstantInt::get(Type::Int32Ty, 1);
      Value* more = BinaryOperator::CreateShl(obj32, one, "shl", block);
      Value* tagged = BinaryOperator::CreateOr(more, one, "or", block);

      return CastInst::Create(
          Instruction::IntToPtr, tagged, ObjType, "as_obj", block);
    }

    void visit_meta_send_op_lt() {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = block("fast");
      BasicBlock* dispatch = block("dispatch");
      BasicBlock* cont = block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      Value* called_value = simple_send(ls_->symbol("<"), 1, dispatch);
      check_for_exception_then(called_value, cont, dispatch);

      ICmpInst* cmp = new ICmpInst(ICmpInst::ICMP_SLT,
          recv, arg, "imm_cmp", fast);
      Value* imm_value = SelectInst::Create(cmp, constant(Qtrue, fast),
          constant(Qfalse, fast), "select_bool", fast);

      BranchInst::Create(cont, fast);

      block_ = cont;

      PHINode* phi = PHINode::Create(ObjType, "addition", block_);
      phi->addIncoming(called_value, dispatch);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_gt() {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = block("fast");
      BasicBlock* dispatch = block("dispatch");
      BasicBlock* cont = block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      Value* called_value = simple_send(ls_->symbol(">"), 1, dispatch);
      check_for_exception_then(called_value, cont, dispatch);

      ICmpInst* cmp = new ICmpInst(ICmpInst::ICMP_SGT,
          recv, arg, "imm_cmp", fast);
      Value* imm_value = SelectInst::Create(cmp, constant(Qtrue, fast),
          constant(Qfalse, fast), "select_bool", fast);

      BranchInst::Create(cont, fast);

      block_ = cont;

      PHINode* phi = PHINode::Create(ObjType, "compare", block_);
      phi->addIncoming(called_value, dispatch);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_plus() {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = block("fast");
      BasicBlock* dispatch = block("dispatch");
      BasicBlock* tagnow = block("tagnow");
      BasicBlock* cont = block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      Value* called_value = simple_send(ls_->symbol("+"), 1, dispatch);
      check_for_exception_then(called_value, cont, dispatch);

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

    void visit_meta_send_op_minus() {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = block("fast");
      BasicBlock* dispatch = block("dispatch");
      BasicBlock* cont = block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      Value* called_value = simple_send(ls_->symbol("-"), 1, dispatch);
      check_for_exception_then(called_value, cont, dispatch);

      std::vector<const Type*> types;
      types.push_back(Int31Ty);
      types.push_back(Int31Ty);

      std::vector<const Type*> struct_types;
      struct_types.push_back(Int31Ty);
      struct_types.push_back(Type::Int1Ty);

      StructType* st = StructType::get(struct_types);

      FunctionType* ft = FunctionType::get(st, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("llvm.ssub.with.overflow.i31", ft));

      Value* recv_int = tag_strip(recv, fast);
      Value* arg_int = tag_strip(arg, fast);
      Value* call_args[] = { recv_int, arg_int };
      Value* res = CallInst::Create(func, call_args, call_args+2, "sub.overflow", fast);

      Value* sum = ExtractValueInst::Create(res, 0, "sub", fast);
      Value* dof = ExtractValueInst::Create(res, 1, "did_overflow", fast);

      BasicBlock* tagnow = block("tagnow");
      BranchInst::Create(dispatch, tagnow, dof, fast);

      Value* imm_value = fixnum_tag(sum, tagnow);

      BranchInst::Create(cont, tagnow);

      block_ = cont;

      PHINode* phi = PHINode::Create(ObjType, "subtraction", block_);
      phi->addIncoming(called_value, dispatch);
      phi->addIncoming(imm_value, tagnow);

      stack_remove(2);
      stack_push(phi);
    }


    Object* literal(opcode which) {
      return vmm_->original.get()->literals()->at(which);
    }

    void visit_push_literal(opcode which) {
      Object* lit = literal(which);
      if(Symbol* sym = try_as<Symbol>(lit)) {
        stack_push(constant(sym));
      } else {
        Value* idx[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, offset::cf_cm)
        };

        Value* gep = GetElementPtrInst::Create(call_frame_, idx, idx+2, "cm_pos", block_);
        Value* cm =  new LoadInst(gep, "cm", block_);

        idx[1] = ConstantInt::get(Type::Int32Ty, 13);
        gep = GetElementPtrInst::Create(cm, idx, idx+2, "literals_pos", block_);
        Value* lits = new LoadInst(gep, "literals", block_);

        Value* idx2[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, offset::tuple_field),
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
        ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
        ConstantInt::get(Type::Int32Ty, which)
      };

      Value* pos = GetElementPtrInst::Create(scope(), idx2, idx2+3, "local_pos", block_);

      stack_push(new LoadInst(pos, "local", block_));
    }

    void visit_set_local(opcode which) {
      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
        ConstantInt::get(Type::Int32Ty, which)
      };

      Value* s = scope();

      Value* pos = GetElementPtrInst::Create(s, idx2, idx2+3, "local_pos", block_);

      Value* val = stack_top();

      new StoreInst(val, pos, false, block_);

      write_barrier(s, val);
    }

    void visit_push_self() {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_self)
      };

      Value* pos = GetElementPtrInst::Create(scope(), idx, idx + 2, "self_pos", block_);

      stack_push(new LoadInst(pos, "self", block_));
    }

    void visit_allow_private() {
      allow_private_ = true;
    }

    void novisit_set_call_flags(opcode flag) {
      call_flags_ = flag;
    }

    void visit_send_stack(opcode which, opcode args) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      if(cache->execute == Primitives::tuple_at && args == 1) {
        Value* recv = stack_back(1);

        Value* flag_idx[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, 0)
        };

        Value* gep = GetElementPtrInst::Create(recv, flag_idx, flag_idx+4, "flag_pos", block_);
        Value* flags = new LoadInst(gep, "flags", block_);

        Value* mask = ConstantInt::get(Type::Int32Ty, (1 << 8) - 1);
        Value* obj_type = BinaryOperator::CreateAnd(flags, mask, "mask", block_);

        Value* tag = ConstantInt::get(Type::Int32Ty, rubinius::Tuple::type);
        Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, obj_type, tag, "is_tuple", block_);

        BasicBlock* is_tuple = block("is_tuple");
        BasicBlock* is_other = block("is_other");
        BasicBlock* cont =     block("continue");

        BranchInst::Create(is_tuple, is_other, cmp, block_);

        block_ = is_tuple;

        Value* index = tag_strip(stack_pop(), block_, Type::Int32Ty);
        stack_remove(1);

        const Type* tuple_type = ptr_type("Tuple");

        Value* tup = CastInst::Create(
          Instruction::BitCast,
          recv,
          tuple_type, "as_tuple", block_);

        Value* idx[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, offset::tuple_field),
          index
        };

        gep = GetElementPtrInst::Create(tup, idx, idx+3, "field_pos", block_);

        stack_push(new LoadInst(gep, "tuple_at", block_));

        BranchInst::Create(cont, block_);

        block_ = is_other;

        Value* ret = inline_cache_send(cache->name, args, cache, block_, allow_private_);
        stack_remove(args + 1);
        check_for_exception(ret);

        stack_push(ret);

        BranchInst::Create(cont, block_);
        block_ = cont;
      } else {
        Value* ret = inline_cache_send(cache->name, args, cache, block_, allow_private_);
        stack_remove(args + 1);
        check_for_exception(ret);
        stack_push(ret);
      }
      allow_private_ = false;
    }

    void visit_send_method(opcode which) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = inline_cache_send(cache->name, 0, cache, block_, allow_private_);
      stack_remove(1);
      check_for_exception(ret);
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
      check_for_return(ret);
      allow_private_ = false;
    }

    void visit_send_stack_with_splat(opcode which, opcode args) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = splat_send(cache->name, args, block_, allow_private_);
      stack_remove(args + 3);
      check_for_exception(ret);
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
      // We only JIT normal methods right now, which only use scope, never
      // top_scope
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::cf_scope)
      };

      Value* gep = GetElementPtrInst::Create(call_frame_, idx, idx+2, "scope_pos", block_);
      Value* ts =  new LoadInst(gep, "scope", block_);

      idx[1] = ConstantInt::get(Type::Int32Ty, 1);
      gep = GetElementPtrInst::Create(ts, idx, idx+2, "block_pos", block_);
      stack_push(new LoadInst(gep, "block", block_));
    }

    void visit_send_super_stack_with_block(opcode which, opcode args) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = super_send(cache->name, args);
      stack_remove(args + 1);
      check_for_return(ret);
    }

    void visit_send_super_stack_with_splat(opcode which, opcode args) {
      SendSite::Internal* cache = reinterpret_cast<SendSite::Internal*>(which);
      Value* ret = super_send(cache->name, args, block_, true);
      stack_remove(args + 2);
      check_for_exception(ret);
      stack_push(ret);
    }

    void visit_add_scope() {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_add_scope", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_pop()
      };

      CallInst::Create(func, call_args, call_args+3, "add_array", block_);
    }

    void visit_push_const_fast(opcode name, opcode cache) {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(Type::Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_const_fast", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(as<Symbol>(literal(name))),
        ConstantInt::get(Type::Int32Ty, cache)
      };

      Value* ret = CallInst::Create(func, call_args, call_args+4, "push_const_fast", block_);
      check_for_exception(ret);
      stack_push(ret);
    }

    void visit_push_const(opcode name) {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_const", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(as<Symbol>(literal(name)))
      };

      Value* ret = CallInst::Create(func, call_args, call_args+3, "push_const_fast", block_);
      check_for_exception(ret);
      stack_push(ret);
    }

    void visit_set_const(opcode name) {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_const", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(as<Symbol>(literal(name))),
        stack_top()
      };

      CallInst::Create(func, call_args, call_args+4, "set_const", block_);
    }

    void visit_set_const_at(opcode name) {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_const_at", ft));

      Value* val = stack_pop();
      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(as<Symbol>(literal(name))),
        stack_top(),
        val
      };

      CallInst::Create(func, call_args, call_args+5, "set_const", block_);

      stack_push(val);
    }

    void visit_set_literal(opcode which) {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(Type::Int32Ty);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_literal", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, which),
        stack_top()
      };

      CallInst::Create(func, call_args, call_args+4, "set_literal", block_);
    }

    void visit_push_variables() {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::cf_scope)
      };

      Value* gep = GetElementPtrInst::Create(call_frame_, idx, idx+2, "vars_pos", block_);
      stack_push(new LoadInst(gep, "vars", block_));
    }

    void visit_push_scope() {
      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::cf_cm)
      };

      Value* gep2 = GetElementPtrInst::Create(call_frame_,
          idx2, idx2+2, "cm_pos", block_);

      Value* cm = new LoadInst(gep2, "cm", block_);

      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::cm_static_scope)
      };

      Value* gep = GetElementPtrInst::Create(cm, idx, idx+2, "scope_pos", block_);
      stack_push(new LoadInst(gep, "scope", block_));
    }

    void visit_cast_for_single_block_arg() {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_cast_for_single_block_arg", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_pop()
      };

      stack_push(CallInst::Create(func, call_args, call_args+3, "cfsba", block_));
    }

    void visit_cast_for_multi_block_arg() {
      Signature sig(ls_, ObjType);
      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_pop()
      };

      Value* val = sig.call("rbx_cast_for_multi_block_arg", call_args, 3,
                            "cfmba", block_);
      stack_push(val);
    }

    void visit_cast_for_splat_block_arg() {
      Signature sig(ls_, ObjType);
      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_pop()
      };

      Value* val = sig.call("rbx_cast_for_splat_block_arg", call_args, 3,
                            "cfmba", block_);
      stack_push(val);
    }

    void visit_set_local_depth(opcode depth, opcode index) {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(Type::Int32Ty);
      types.push_back(Type::Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_local_depth", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_pop(),
        ConstantInt::get(Type::Int32Ty, depth),
        ConstantInt::get(Type::Int32Ty, index)
      };

      stack_push(CallInst::Create(func, call_args, call_args+5, "sld", block_));
    }

    void visit_push_local_depth(opcode depth, opcode index) {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(Type::Int32Ty);
      types.push_back(Type::Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_local_depth", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, depth),
        ConstantInt::get(Type::Int32Ty, index)
      };

      stack_push(CallInst::Create(func, call_args, call_args+4, "pld", block_));
    }

    void visit_goto(opcode ip) {
      BranchInst::Create(block_map_[ip], block_);
      block_ = block("continue");
    }

    void visit_goto_if_true(opcode ip) {
      Value* cond = stack_pop();
      Value* i = CastInst::Create(
          Instruction::PtrToInt,
          cond, IntPtrTy, "as_int", block_);

      Value* anded = BinaryOperator::CreateAnd(i,
          ConstantInt::get(Type::Int32Ty, FALSE_MASK), "and", block_);

      Value* cmp = new ICmpInst(ICmpInst::ICMP_NE, anded,
          ConstantInt::get(Type::Int32Ty, cFalse), "is_true", block_);

      BasicBlock* cont = block("continue");
      BranchInst::Create(block_map_[ip], cont, cmp, block_);

      block_ = cont;
    }

    void visit_goto_if_false(opcode ip) {
      Value* cond = stack_pop();
      Value* i = CastInst::Create(
          Instruction::PtrToInt,
          cond, IntPtrTy, "as_int", block_);

      Value* anded = BinaryOperator::CreateAnd(i,
          ConstantInt::get(Type::Int32Ty, FALSE_MASK), "and", block_);

      Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, anded,
          ConstantInt::get(Type::Int32Ty, cFalse), "is_true", block_);

      BasicBlock* cont = block("continue");
      BranchInst::Create(block_map_[ip], cont, cmp, block_);

      block_ = cont;
    }

    void visit_yield_stack(opcode count) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << Type::Int32Ty;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, count),
        stack_objects(count)
      };

      Value* val = sig.call("rbx_yield_stack", call_args, 4, "ys", block_);
      stack_remove(count);

      check_for_exception(val);
      stack_push(val);
    }

    void visit_yield_splat(opcode count) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << Type::Int32Ty;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, count),
        stack_objects(count + 1)
      };

      Value* val = sig.call("rbx_yield_splat", call_args, 4, "ys", block_);
      stack_remove(count + 1);

      check_for_exception(val);
      stack_push(val);
    }

    void visit_check_interrupts() {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_check_interrupts", ft));

      Value* call_args[] = {
        vm_,
        call_frame_
      };

      Value* ret = CallInst::Create(func, call_args, call_args+2, "ci", block_);
      check_for_exception(ret);
    }

    void visit_check_serial(opcode index, opcode serial) {
        std::vector<const Type*> types;

        types.push_back(VMTy);
        types.push_back(CallFrameTy);
        types.push_back(Type::Int32Ty);
        types.push_back(Type::Int32Ty);
        types.push_back(ObjType);

        FunctionType* ft = FunctionType::get(ObjType, types, false);
        Function* func = cast<Function>(
            module_->getOrInsertFunction("rbx_check_serial", ft));

        Value* call_args[] = {
          vm_,
          call_frame_,
          ConstantInt::get(Type::Int32Ty, index),
          ConstantInt::get(Type::Int32Ty, serial),
          stack_pop()
        };

        stack_push(CallInst::Create(func, call_args, call_args+5, "cs", block_));
    }

    void visit_push_my_offset(opcode i) {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_self)
      };

      Value* pos = GetElementPtrInst::Create(scope(), idx, idx + 2, "self_pos", block_);

      Value* self = new LoadInst(pos, "self", block_);

      assert(i % sizeof(Object*) == 0);

      Value* cst = CastInst::Create(
          Instruction::BitCast,
          self,
          PointerType::getUnqual(ObjType), "obj_array", block_);

      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, i / sizeof(Object*))
      };

      pos = GetElementPtrInst::Create(cst, idx2, idx2+1, "val_pos", block_);

      stack_push(new LoadInst(pos, "val", block_));
    }

    void visit_setup_unwind(opcode where, opcode type) {
      BasicBlock* code;
      if(type == cRescue) {
        code = block("is_exception");
        std::vector<const Type*> types;
        types.push_back(VMTy);

        FunctionType* ft = FunctionType::get(Type::Int1Ty, types, false);
        Function* func = cast<Function>(
            module_->getOrInsertFunction("rbx_raising_exception", ft));

        Value* call_args[] = { vm_ };
        Value* isit = CallInst::Create(func, call_args, call_args+1, "rae", code);

        BasicBlock* next = 0;
        if(exception_handlers_.size() == 0) {
          next = bail_out_;
        } else {
          next = exception_handlers_.back()->code();
        }

        BranchInst::Create(block_map_[where], next, isit, code);
      } else {
        code = block_map_[where];
      }

      // Reset the stack pointer within the handler to the current value
      set_stack_ptr(stack_ptr(), block_map_[where]);

      ExceptionHandler* handler = new ExceptionHandler(code);
      exception_handlers_.push_back(handler);
    }

    void visit_pop_unwind() {
      ExceptionHandler* handler = exception_handlers_.back();
      exception_handlers_.pop_back();

      delete handler;
    }

    void visit_reraise() {
      if(exception_handlers_.size() > 0) {
        BranchInst::Create(exception_handlers_.back()->code(), block_);
      } else {
        BranchInst::Create(bail_out_, block_);
      }
      block_ = block("continue");
    }

    void visit_raise_return() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_top()
      };

      sig.call("rbx_raise_return", call_args, 3, "raise_return", block_);
      visit_reraise();
    }

    void visit_ensure_return() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_top()
      };

      sig.call("rbx_ensure_return", call_args, 3, "ensure_return", block_);
      visit_reraise();
    }

    void visit_raise_break() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_top()
      };

      sig.call("rbx_raise_break", call_args, 3, "raise_break", block_);
      visit_reraise();
    }

    void visit_push_exception() {
      std::vector<const Type*> types;

      types.push_back(VMTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_current_exception", ft));

      Value* call_args[] = { vm_ };

      stack_push(CallInst::Create(func, call_args, call_args+1, "ce", block_));
    }

    void visit_clear_exception() {
      std::vector<const Type*> types;

      types.push_back(VMTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_clear_exception", ft));

      Value* call_args[] = { vm_ };

      CallInst::Create(func, call_args, call_args+1, "ce", block_);
    }

    void visit_pop_exception() {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_pop_exception", ft));

      Value* call_args[] = { vm_, stack_pop() };

      CallInst::Create(func, call_args, call_args+2, "pe", block_);
    }

    void visit_find_const(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << Type::Int32Ty;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, which),
        stack_pop()
      };

      Value* val = sig.call("rbx_find_const", call_args, 4, "constant", block_);
      stack_push(val);
    }

    void visit_instance_of() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << ObjType;

      Value* top = stack_pop();
      Value* call_args[] = {
        vm_,
        call_frame_,
        top,
        stack_pop()
      };

      Value* val = sig.call("rbx_instance_of", call_args, 4, "constant", block_);
      stack_push(val);
    }

    void visit_kind_of() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << ObjType;

      Value* top = stack_pop();
      Value* call_args[] = {
        vm_,
        call_frame_,
        top,
        stack_pop()
      };

      Value* val = sig.call("rbx_kind_of", call_args, 4, "constant", block_);
      stack_push(val);
    }

    void visit_is_nil() {
      Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, stack_pop(),
          constant(Qnil), "is_nil", block_);
      Value* imm_value = SelectInst::Create(cmp, constant(Qtrue),
          constant(Qfalse), "select_bool", block_);
      stack_push(imm_value);
    }

    void visit_make_array(opcode count) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << Type::Int32Ty;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, count),
        stack_objects(count)
      };

      Value* val = sig.call("rbx_make_array", call_args, 4, "constant", block_);
      stack_remove(count);
      stack_push(val);
    }

    void visit_meta_send_call(opcode count) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << Type::Int32Ty;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, count),
        stack_objects(count + 1)
      };

      Value* val = sig.call("rbx_meta_send_call", call_args, 4, "constant", block_);
      stack_remove(count+1);
      check_for_exception(val);
      stack_push(val);
    }

    void visit_passed_arg(opcode count) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << "Arguments";
      sig << Type::Int32Ty;

      Value* call_args[] = {
        vm_,
        args_,
        ConstantInt::get(Type::Int32Ty, count)
      };

      Value* val = sig.call("rbx_passed_arg", call_args, 3, "pa", block_);
      stack_push(val);
    }

    void visit_passed_blockarg(opcode count) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << "Arguments";
      sig << Type::Int32Ty;

      Value* call_args[] = {
        vm_,
        args_,
        ConstantInt::get(Type::Int32Ty, count)
      };

      Value* val = sig.call("rbx_passed_blockarg", call_args, 3, "pa", block_);
      stack_push(val);
    }

    void visit_push_cpath_top() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << Type::Int32Ty;

      Value* call_args[] = {
        vm_,
        ConstantInt::get(Type::Int32Ty, 0)
      };

      Value* val = sig.call("rbx_push_system_object", call_args, 2, "so", block_);
      stack_push(val);
    }

    void visit_push_ivar(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(as<Symbol>(literal(which)))
      };

      Value* val = sig.call("rbx_push_ivar", call_args, 3, "ivar", block_);
      check_for_exception(val);
      stack_push(val);
    }

    void visit_set_ivar(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(as<Symbol>(literal(which))),
        stack_top()
      };

      sig.call("rbx_set_ivar", call_args, 4, "ivar", block_);
    }

    void visit_push_my_field(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << Type::Int32Ty;

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, which)
      };

      Value* val = sig.call("rbx_push_my_field", call_args, 3, "field", block_);
      check_for_exception(val);
      stack_push(val);
    }

    void visit_store_my_field(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << Type::Int32Ty;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(Type::Int32Ty, which),
        stack_top()
      };

      sig.call("rbx_set_my_field", call_args, 4, "field", block_);
    }

    void visit_shift_array() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_back_position(0)
      };

      Value* val = sig.call("rbx_shift_array", call_args, 3, "field", block_);
      stack_push(val);
    }

    void visit_string_append() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << ObjType;

      Value* val = stack_pop();

      Value* call_args[] = {
        vm_,
        call_frame_,
        val,
        stack_pop()
      };

      Value* str = sig.call("rbx_string_append", call_args, 4, "string", block_);
      stack_push(str);
    }
  };
}
