#include "instructions_util.hpp"

#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/global_cache_entry.hpp"
#include "inline_cache.hpp"

#include "llvm/access_memory.hpp"
#include "llvm/jit_operations.hpp"
#include "llvm/inline.hpp"

#include <llvm/DerivedTypes.h>

#include <list>

namespace rubinius {

  typedef std::map<int, llvm::BasicBlock*> BlockMap;

  typedef std::list<llvm::BasicBlock*> EHandlers;

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

  class JITVisit : public VisitInstructions<JITVisit>, public JITOperations {
    JITFunctions f;
    BlockMap block_map_;

    llvm::Value* stack_;
    llvm::Value* call_frame_;
    llvm::Function* function_;

    llvm::Value* stack_top_;

    bool allow_private_;
    opcode call_flags_;

    // Cached Function*s
    llvm::Function* rbx_simple_send_;
    llvm::Function* rbx_simple_send_private_;

    // bail out destinations
    llvm::BasicBlock* bail_out_;
    llvm::BasicBlock* bail_out_fast_;

    EHandlers exception_handlers_;

    Value* method_entry_;
    Value* args_;

    Value* ip_pos_;

    Value* vars_;
    bool creates_blocks_;

    bool is_block_;

    Value* global_serial_pos;

    // The single Arguments object on the stack, plus positions into it
    // that we store the call info
    Value* out_args_;
    Value* out_args_recv_;
    Value* out_args_block_;
    Value* out_args_total_;
    Value* out_args_arguments_;
    Value* out_args_array_;

  public:

    class Unsupported {};

    void init_out_args(BasicBlock* block) {
      out_args_ = new AllocaInst(type("Arguments"), 0, "out_args", block);

      out_args_recv_ = ptr_gep(out_args_, 0, "out_args_recv", block);
      out_args_block_= ptr_gep(out_args_, 1, "out_args_block", block);
      out_args_total_= ptr_gep(out_args_, 2, "out_args_total", block);
      out_args_arguments_ = ptr_gep(out_args_, 3, "out_args_arguments", block);
      out_args_array_ = ptr_gep(out_args_, 4, "out_args_array", block);
    }

    JITVisit(LLVMState* ls, VMMethod* vmm,
             llvm::Module* mod, llvm::Function* func, llvm::BasicBlock* start,
             llvm::Value* stack, llvm::Value* call_frame,
             llvm::Value* stack_top, llvm::Value* me, llvm::Value* args,
             llvm::Value* vars, bool is_block)
      : JITOperations(ls, vmm, mod, stack_top, start, func)
      , f(ls)
      , stack_(stack)
      , call_frame_(call_frame)
      , function_(func)
      , stack_top_(stack_top)
      , allow_private_(false)
      , call_flags_(0)
      , rbx_simple_send_(0)
      , rbx_simple_send_private_(0)
      , method_entry_(me)
      , args_(args)
      , vars_(vars)
      , creates_blocks_(true)
      , is_block_(is_block)
    {
      bail_out_ = new_block("bail_out");

      Value* call_args[] = {
        vm_,
        call_frame_
      };

      Value* isit = f.return_to_here.call(call_args, 2, "rth", bail_out_);

      BasicBlock* ret_raise_val = new_block("ret_raise_val");
      bail_out_fast_ = new_block("ret_null");

      block_->moveAfter(bail_out_fast_);

      BranchInst::Create(ret_raise_val, bail_out_fast_, isit, bail_out_);

      ReturnInst::Create(Constant::getNullValue(ObjType), bail_out_fast_);

      Value* crv = f.clear_raise_value.call(&vm_, 1, "crv", ret_raise_val);
      ReturnInst::Create(crv, ret_raise_val);

      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::cf_ip)
      };


      ip_pos_ = GetElementPtrInst::Create(call_frame_, idx, idx+2, "ip_pos", block_);

      global_serial_pos = CastInst::Create(
          Instruction::IntToPtr,
          ConstantInt::get(IntPtrTy, (intptr_t)ls_->shared().global_serial_address()),
          PointerType::getUnqual(IntPtrTy), "cast_to_intptr", block_);

      init_out_args(block_);
    }

    void set_creates_blocks(bool val) {
      creates_blocks_ = val;
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

    BlockMap& block_map() {
      return block_map_;
    }

    void check_for_return(Value* val) {
      BasicBlock* cont = new_block();
      BasicBlock* push_val = new_block("push_val");

      Value* null = Constant::getNullValue(ObjType);

      Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, val, null, "null_check", block_);
      BasicBlock* is_break = new_block("is_break");
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

      BasicBlock* push_break_val = new_block("push_break_val");
      BasicBlock* next = 0;

      // If there are handlers...
      if(exception_handlers_.size() > 0) {
        next = exception_handlers_.back();
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
        BasicBlock* handler = exception_handlers_.back();
        BranchInst::Create(handler, cont, cmp, block);
      } else {
        BranchInst::Create(bail_out_fast_, cont, cmp, block);
      }
    }

    BasicBlock* check_for_exception(Value* val, BasicBlock* els) {
      BasicBlock* cont = new_block();
      check_for_exception_then(val, cont, els);

      return cont;
    }

    void check_for_exception(Value* val) {
      block_ = check_for_exception(val, block_);
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
        BasicBlock* end_profiling = new_block("end_profiling");
        BasicBlock* cont = new_block("continue");

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

      BasicBlock* right_check = new_block("ref_check");
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

    void setup_out_args(int args, BasicBlock* block) {
      new StoreInst(stack_back(args, block), out_args_recv_, false, block);
      new StoreInst(constant(Qnil, block), out_args_block_, false, block);
      new StoreInst(ConstantInt::get(Type::Int32Ty, args),
                    out_args_total_, false, block);
      new StoreInst(stack_objects(args, block), out_args_arguments_, false, block);
      new StoreInst(Constant::getNullValue(ptr_type("Array")),
                    out_args_array_, false, block);
    }

    void setup_out_args_with_block(int args, BasicBlock* block) {
      new StoreInst(stack_back(args + 1, block), out_args_recv_, false, block);
      new StoreInst(stack_top(block), out_args_block_, false, block);
      new StoreInst(ConstantInt::get(Type::Int32Ty, args),
                    out_args_total_, false, block);
      new StoreInst(stack_objects(args + 1, block), out_args_arguments_, false, block);
      new StoreInst(Constant::getNullValue(ptr_type("Array")),
                    out_args_array_, false, block);
    }

    Value* inline_cache_send(int args, InlineCache* cache,
                             BasicBlock* block=NULL)
    {
      if(!block) block = block_;

      Value* cache_const = CastInst::Create(
          Instruction::IntToPtr,
          ConstantInt::get(IntPtrTy, reinterpret_cast<uintptr_t>(cache)),
          ptr_type("InlineCache"), "cast_to_ptr", block);

      Value* execute_pos_idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 3),
      };

      Value* execute_pos = GetElementPtrInst::Create(cache_const,
          execute_pos_idx, execute_pos_idx+2, "execute_pos", block);

      Value* execute = new LoadInst(execute_pos, "execute", block);

      setup_out_args(args, block);

      Value* call_args[] = {
        vm_,
        cache_const,
        call_frame_,
        out_args_
      };

      return CallInst::Create(execute, call_args, call_args+4, "ic_send", block);
    }

    Value* block_send(InlineCache* cache, int args,
                      BasicBlock* block=NULL, bool priv=false)
    {
      if(!block) block = block_;

      Value* cache_const = CastInst::Create(
          Instruction::IntToPtr,
          ConstantInt::get(IntPtrTy, reinterpret_cast<uintptr_t>(cache)),
          ptr_type("InlineCache"), "cast_to_ptr", block);

      Value* execute_pos_idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 3),
      };

      Value* execute_pos = GetElementPtrInst::Create(cache_const,
          execute_pos_idx, execute_pos_idx+2, "execute_pos", block);

      Value* execute = new LoadInst(execute_pos, "execute", block);

      setup_out_args_with_block(args, block);

      Value* call_args[] = {
        vm_,
        cache_const,
        call_frame_,
        out_args_
      };

      return CallInst::Create(execute, call_args, call_args+4, "ic_send", block);
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

    void visit_meta_send_op_equal(opcode name) {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block();

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

    void visit_meta_send_op_tequal(opcode name) {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block();

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

    void visit_meta_send_op_lt(opcode name) {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block("cont");

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

    void visit_meta_send_op_gt(opcode name) {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block("cont");

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

    void visit_meta_send_op_plus(opcode name) {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* tagnow = new_block("tagnow");
      BasicBlock* cont = new_block("cont");

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

    void visit_meta_send_op_minus(opcode name) {
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block("cont");

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

      BasicBlock* tagnow = new_block("tagnow");
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

    Value* get_literal(opcode which) {
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
      return new LoadInst(gep, "literal", block_);
    }

    void visit_push_literal(opcode which) {
      Object* lit = literal(which);
      if(Symbol* sym = try_as<Symbol>(lit)) {
        stack_push(constant(sym));
      } else {
        stack_push(get_literal(which));
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

    void push_local(Value* scope, opcode which) {
      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
        ConstantInt::get(Type::Int32Ty, which)
      };

      Value* pos = GetElementPtrInst::Create(scope, idx2, idx2+3, "local_pos", block_);

      stack_push(new LoadInst(pos, "local", block_));
    }

    void visit_push_local(opcode which) {
      Value* vars;
      if(is_block_) {
        vars = top_scope();
      } else if(creates_blocks_) {
        vars = scope();
      } else {
        vars = vars_;
      }

      push_local(vars, which);
    }

    void set_local(Value* scope, opcode which, bool use_wb=true) {
      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_tuple),
        ConstantInt::get(Type::Int32Ty, which)
      };

      Value* pos = GetElementPtrInst::Create(scope, idx2, idx2+3, "local_pos", block_);

      Value* val = stack_top();

      new StoreInst(val, pos, false, block_);

      if(use_wb) write_barrier(scope, val);
    }

    void visit_set_local(opcode which) {
      bool use_wb;
      Value* vars;

      if(is_block_) {
        vars = top_scope();
        use_wb = true;
      } else if(creates_blocks_) {
        vars = scope();
        use_wb = true;
      } else {
        vars = vars_;
        use_wb = false;
      }

      set_local(vars, which, use_wb);
    }

    Value* get_self() {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_self)
      };

      Value* pos = GetElementPtrInst::Create(scope(), idx, idx + 2, "self_pos", block_);

      return new LoadInst(pos, "self", block_);
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

    void emit_uncommon() {
      Signature sig(ls_, "Object");

      sig << "VM";
      sig << "CallFrame";
      sig << "Arguments";
      sig << IntPtrTy;

      Value* cur = stack_ptr();

      Value* sp = subtract_pointers(cur, stack_);

      Value* call_args[] = { vm_, call_frame_, args_, sp };

      Value* call = sig.call("rbx_continue_uncommon", call_args, 4, "", block_);

      ReturnInst::Create(call, block_);
    }

    void visit_send_stack(opcode which, opcode args) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
      BasicBlock* after = new_block("after_send");

      if(cache->method) {
        Inliner inl(*this, cache, args, after);
        // Uncommon doesn't yet know how to synthesize UnwindInfos, so
        // don't do uncommon if there are handlers.
        if(inl.consider() && exception_handlers_.size() == 0) {
          emit_uncommon();

          block_ = after;

          allow_private_ = false;
          return;
        }
      }

      Value* ret = inline_cache_send(args, cache, block_);
      stack_remove(args + 1);
      check_for_exception(ret);
      stack_push(ret);

      BranchInst::Create(after, block_);
      block_ = after;

      allow_private_ = false;
    }

    /*
     * This is causing a minor slowdown, shows up on
     * running an empty times loop, so it's not used atm. */
    void call_is_fixnum() {
      Value* index_val = stack_pop();

      Value* fix_mask = ConstantInt::get(IntPtrTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(index_val);
      Value* masked = BinaryOperator::CreateAnd(lint, fix_mask, "masked", block_);

      Value* fix_cmp = new ICmpInst(ICmpInst::ICMP_EQ, masked, fix_tag, "is_fixnum", block_);
      Value* imm_value = SelectInst::Create(fix_cmp, constant(Qtrue),
          constant(Qfalse), "select_bool", block_);
      stack_push(imm_value);
    }

    void visit_send_method(opcode which) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
      BasicBlock* after = new_block("after_send");

      if(cache->method) {
        Inliner inl(*this, cache, 0, after);
        if(inl.consider() && exception_handlers_.size() == 0) {
          emit_uncommon();

          block_ = after;

          allow_private_ = false;
          return;
        }
      }

      Value* ret = inline_cache_send(0, cache, block_);
      stack_remove(1);
      check_for_exception(ret);
      stack_push(ret);

      BranchInst::Create(after, block_);
      block_ = after;

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
      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
      Value* ret = block_send(cache, args, block_, allow_private_);
      stack_remove(args + 2);
      check_for_return(ret);
      allow_private_ = false;
    }

    void visit_send_stack_with_splat(opcode which, opcode args) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
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
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::cf_scope)
      };

      // We're JITing a block, use top_scope
      if(is_block_) {
        idx[1] = ConstantInt::get(Type::Int32Ty, offset::cf_top_scope);
      }

      Value* gep = GetElementPtrInst::Create(call_frame_, idx, idx+2, "scope_pos", block_);
      Value* ts =  new LoadInst(gep, "scope", block_);

      idx[1] = ConstantInt::get(Type::Int32Ty, 1);
      gep = GetElementPtrInst::Create(ts, idx, idx+2, "block_pos", block_);
      stack_push(new LoadInst(gep, "block", block_));
    }

    void visit_send_super_stack_with_block(opcode which, opcode args) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
      Value* ret = super_send(cache->name, args);
      stack_remove(args + 1);
      check_for_return(ret);
    }

    void visit_send_super_stack_with_splat(opcode which, opcode args) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
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

    Object* current_literal(opcode which) {
      return vmm_->original.get()->literals()->at(which);
    }

    void visit_push_const_fast(opcode name, opcode cache) {
      AccessManagedMemory memguard(ls_);

      BasicBlock* cont = 0;

      GlobalCacheEntry* entry = try_as<GlobalCacheEntry>(current_literal(cache));
      if(entry) {
        assert(entry->pin());

        Value* global_serial = new LoadInst(global_serial_pos, "global_serial", block_);

        Value* current_serial_pos = CastInst::Create(
            Instruction::IntToPtr,
            ConstantInt::get(IntPtrTy, (intptr_t)entry->serial_location()),
            PointerType::getUnqual(IntPtrTy), "cast_to_intptr", block_);

        Value* current_serial = new LoadInst(current_serial_pos, "serial", block_);

        Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, global_serial,
            current_serial, "use_cache", block_);

        BasicBlock* use_cache = new_block("use_cache");
        BasicBlock* use_call  = new_block("use_call");
        cont =      new_block("continue");

        BranchInst::Create(use_cache, use_call, cmp, block_);

        block_ = use_cache;

        Value* value_pos = CastInst::Create(
            Instruction::IntToPtr,
            ConstantInt::get(IntPtrTy, (intptr_t)entry->value_location()),
            PointerType::getUnqual(ObjType), "cast_to_objptr", block_);

        stack_push(new LoadInst(value_pos, "cached_value", block_));

        BranchInst::Create(cont, block_);

        block_ = use_call;
      }

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

      if(entry) {
        BranchInst::Create(cont, block_);

        block_ = cont;
      }
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
      types.push_back(ObjType);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_const_at", ft));

      Value* val = stack_pop();
      Value* call_args[] = {
        vm_,
        constant(as<Symbol>(literal(name))),
        stack_top(),
        val
      };

      CallInst::Create(func, call_args, call_args+4, "set_const", block_);

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
      Signature sig(ls_, ObjType);
      sig << "VM";
      sig << "CallFrame";

      Value* args[] = {
        vm_,
        call_frame_
      };

      stack_push(sig.call("rbx_promote_variables", args, 2, "promo_vars", block_));
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
      types.push_back(ptr_type("Arguments"));

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_cast_for_single_block_arg", ft));

      Value* call_args[] = {
        vm_,
        args_
      };

      stack_push(CallInst::Create(func, call_args, call_args+2, "cfsba", block_));
    }

    void visit_cast_for_multi_block_arg() {
      Signature sig(ls_, ObjType);
      sig << VMTy;
      sig << ptr_type("Arguments");

      Value* call_args[] = {
        vm_,
        args_
      };

      Value* val = sig.call("rbx_cast_for_multi_block_arg", call_args, 2,
                            "cfmba", block_);
      stack_push(val);
    }

    void visit_cast_for_splat_block_arg() {
      Signature sig(ls_, ObjType);
      sig << VMTy;
      sig << ptr_type("Arguments");

      Value* call_args[] = {
        vm_,
        args_
      };

      Value* val = sig.call("rbx_cast_for_splat_block_arg", call_args, 2,
                            "cfmba", block_);
      stack_push(val);
    }

    void visit_set_local_depth(opcode depth, opcode index) {
      if(depth == 0) {
        set_local(scope(), index);
        return;
      } else if(depth == 1) {
        Value* idx[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, offset::vars_parent)
        };

        Value* gep = GetElementPtrInst::Create(scope(), idx, idx+2, "parent_pos", block_);

        Value* parent = new LoadInst(gep, "scope.parent", block_);
        set_local(parent, index);
        return;
      }

      // Handle depth > 1

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
      if(depth == 0) {
        push_local(scope(), index);
        return;
      } else if(depth == 1) {
        Value* idx[] = {
          ConstantInt::get(Type::Int32Ty, 0),
          ConstantInt::get(Type::Int32Ty, offset::vars_parent)
        };

        Value* gep = GetElementPtrInst::Create(scope(), idx, idx+2, "parent_pos", block_);

        Value* parent = new LoadInst(gep, "scope.parent", block_);
        push_local(parent, index);
        return;
      }

      // Handle depth > 1
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
      block_ = new_block("continue");
    }

    void visit_goto_if_true(opcode ip) {
      Value* cond = stack_pop();
      Value* i = CastInst::Create(
          Instruction::PtrToInt,
          cond, IntPtrTy, "as_int", block_);

      Value* anded = BinaryOperator::CreateAnd(i,
          ConstantInt::get(IntPtrTy, FALSE_MASK), "and", block_);

      Value* cmp = new ICmpInst(ICmpInst::ICMP_NE, anded,
          ConstantInt::get(IntPtrTy, cFalse), "is_true", block_);

      BasicBlock* cont = new_block("continue");
      BranchInst::Create(block_map_[ip], cont, cmp, block_);

      block_ = cont;
    }

    void visit_goto_if_false(opcode ip) {
      Value* cond = stack_pop();
      Value* i = CastInst::Create(
          Instruction::PtrToInt,
          cond, IntPtrTy, "as_int", block_);

      Value* anded = BinaryOperator::CreateAnd(i,
          ConstantInt::get(IntPtrTy, FALSE_MASK), "and", block_);

      Value* cmp = new ICmpInst(ICmpInst::ICMP_EQ, anded,
          ConstantInt::get(IntPtrTy, cFalse), "is_true", block_);

      BasicBlock* cont = new_block("continue");
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

      pos = GetElementPtrInst::Create(cst, idx2, idx2+1, "field_pos", block_);

      stack_push(new LoadInst(pos, "field", block_));
    }

    void visit_setup_unwind(opcode where, opcode type) {
      BasicBlock* code;
      if(type == cRescue) {
        code = new_block("is_exception");
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
          next = exception_handlers_.back();
        }

        BranchInst::Create(block_map_[where], next, isit, code);
      } else {
        code = block_map_[where];
      }

      // Reset the stack pointer within the handler to the current value
      set_stack_ptr(stack_ptr(), block_map_[where]);

      exception_handlers_.push_back(code);
    }

    void visit_pop_unwind() {
      exception_handlers_.pop_back();
    }

    void visit_reraise() {
      if(exception_handlers_.size() > 0) {
        BranchInst::Create(exception_handlers_.back(), block_);
      } else {
        BranchInst::Create(bail_out_, block_);
      }
      block_ = new_block("continue");
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
      sig << ObjType;
      sig << ObjType;

      Value* top = stack_pop();
      Value* call_args[] = {
        vm_,
        top,
        stack_pop()
      };

      Value* val = sig.call("rbx_instance_of", call_args, 3, "constant", block_);
      stack_push(val);
    }

    void visit_kind_of() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjType;
      sig << ObjType;

      Value* top = stack_pop();
      Value* call_args[] = {
        vm_,
        top,
        stack_pop()
      };

      Value* val = sig.call("rbx_kind_of", call_args, 3, "constant", block_);
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
      sig << Type::Int32Ty;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        ConstantInt::get(Type::Int32Ty, count),
        stack_objects(count)
      };

      Value* val = sig.call("rbx_make_array", call_args, 3, "constant", block_);
      stack_remove(count);
      stack_push(val);
    }

    void visit_meta_send_call(opcode name, opcode count) {
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
      sig << ObjType;
      sig << ObjType;

      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_self)
      };

      Value* pos = GetElementPtrInst::Create(scope(), idx, idx + 2, "self_pos", block_);

      Value* self = new LoadInst(pos, "self", block_);

      Value* call_args[] = {
        vm_,
        self,
        constant(as<Symbol>(literal(which)))
      };

      Value* val = sig.call("rbx_push_ivar", call_args, 3, "ivar", block_);
      // TODO: why would rbx_push_ivar raise an exception?
      // check_for_exception(val);
      stack_push(val);
    }

    void visit_set_ivar(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjType;
      sig << ObjType;
      sig << ObjType;

      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_self)
      };

      Value* pos = GetElementPtrInst::Create(scope(), idx, idx + 2, "self_pos", block_);

      Value* self = new LoadInst(pos, "self", block_);

      Value* call_args[] = {
        vm_,
        self,
        constant(as<Symbol>(literal(which))),
        stack_top()
      };

      sig.call("rbx_set_ivar", call_args, 4, "ivar", block_);
    }

    void visit_push_my_field(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjType;
      sig << Type::Int32Ty;

      Value* self = get_self();

      Value* call_args[] = {
        vm_,
        self,
        ConstantInt::get(Type::Int32Ty, which)
      };

      Value* val = sig.call("rbx_push_my_field", call_args, 3, "field", block_);
      check_for_exception(val);
      stack_push(val);
    }

    void visit_store_my_field(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjType;
      sig << Type::Int32Ty;
      sig << ObjType;

      Value* self = get_self();

      Value* call_args[] = {
        vm_,
        self,
        ConstantInt::get(Type::Int32Ty, which),
        stack_top()
      };

      sig.call("rbx_set_my_field", call_args, 4, "field", block_);
    }

    void visit_shift_array() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        stack_back_position(0)
      };

      Value* val = sig.call("rbx_shift_array", call_args, 2, "field", block_);
      stack_push(val);
    }

    void visit_string_append() {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjType;
      sig << ObjType;

      Value* val = stack_pop();

      Value* call_args[] = {
        vm_,
        val,
        stack_pop()
      };

      Value* str = sig.call("rbx_string_append", call_args, 3, "string", block_);
      stack_push(str);
    }
  };
}
