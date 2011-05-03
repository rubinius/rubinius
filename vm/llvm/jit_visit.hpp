#include "instructions_util.hpp"

#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/global_cache_entry.hpp"
#include "inline_cache.hpp"

#include "llvm/access_memory.hpp"
#include "llvm/jit_operations.hpp"
#include "llvm/inline.hpp"
#include "llvm/jit_context.hpp"

#include <llvm/DerivedTypes.h>

#include <list>

#ifdef IS_X8664
#define ADD_WITH_OVERFLOW "llvm.sadd.with.overflow.i63"
#define SUB_WITH_OVERFLOW "llvm.ssub.with.overflow.i63"
#else
#define ADD_WITH_OVERFLOW "llvm.sadd.with.overflow.i31"
#define SUB_WITH_OVERFLOW "llvm.ssub.with.overflow.i31"
#endif

namespace rubinius {
  extern "C" Object* invoke_object_class(STATE, CallFrame* call_frame, Object** args, int arg_count);


  typedef std::list<llvm::BasicBlock*> EHandlers;

  class JITFunction : public Signature {
    llvm::Function* func_;

  public:
    JITFunction(LLVMState* ls)
      : Signature(ls, ls->VoidTy)
      , func_(0)
    {}

    void resolve(const char* name, const Type* rt) {
      ret_type_ = rt;
      func_ = function(name);
    }

    Value* call(Value** start, int size, const char* inst_name, IRBuilder<>& b) {
      return b.CreateCall(func_, start, start+size, inst_name);
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

      return_to_here.resolve("rbx_return_to_here", ls->Int1Ty);

      clear_raise_value
        << "VM";

      clear_raise_value.resolve("rbx_clear_raise_value", ls->object());
    }
  };

  class JITVisit : public VisitInstructions<JITVisit>, public JITOperations {
    JITFunctions f;
    BlockMap& block_map_;

    bool allow_private_;
    opcode call_flags_;

    // Cached Function*s
    llvm::Function* rbx_simple_send_;
    llvm::Function* rbx_simple_send_private_;

    // bail out destinations
    llvm::BasicBlock* bail_out_;
    llvm::BasicBlock* bail_out_fast_;

    Value* method_entry_;
    Value* args_;

    Value* ip_pos_;

    Value* vars_;
    bool use_full_scope_;

    Value* global_serial_pos;

    // The single Arguments object on the stack, plus positions into it
    // that we store the call info
    Value* out_args_;
    Value* out_args_recv_;
    Value* out_args_block_;
    Value* out_args_total_;
    Value* out_args_arguments_;
    Value* out_args_container_;

    int called_args_;
    int sends_done_;
    bool has_side_effects_;

    int current_ip_;
    int current_block_;
    int block_arg_shift_;
    bool skip_yield_stack_;

    JITBasicBlock* current_jbb_;

    Value* vmm_debugging_;

  public:

    static const int cHintLazyBlockArgs = 1;

    class Unsupported {};

    void init_out_args() {
      out_args_ = info().out_args();

      out_args_recv_ = ptr_gep(out_args_, 0, "out_args_recv");
      out_args_block_= ptr_gep(out_args_, 1, "out_args_block");
      out_args_total_= ptr_gep(out_args_, 2, "out_args_total");
      out_args_arguments_ = ptr_gep(out_args_, 3, "out_args_arguments");
      out_args_container_ = ptr_gep(out_args_, offset::args_container,
                                    "out_args_container");
    }

    JITVisit(LLVMState* ls, JITMethodInfo& info, BlockMap& bm,
             llvm::BasicBlock* start)
      : JITOperations(ls, info, start)
      , f(ls)
      , block_map_(bm)
      , allow_private_(false)
      , call_flags_(0)
      , rbx_simple_send_(0)
      , rbx_simple_send_private_(0)
      , method_entry_(info.profiling_entry())
      , args_(info.args())
      , vars_(info.variables())
      , use_full_scope_(false)
      , called_args_(-1)
      , sends_done_(0)
      , has_side_effects_(false)
      , current_ip_(0)
      , current_block_(-1)
      , block_arg_shift_(0)
      , skip_yield_stack_(false)
      , current_jbb_(0)
    {}

    bool for_inlined_method() {
      return info().for_inlined_method();
    }

    void initialize() {
      BasicBlock* start = b().GetInsertBlock();

      bail_out_ = new_block("bail_out");

      Value* call_args[] = {
        vm_,
        call_frame_
      };

      set_block(bail_out_);

      Value* isit = f.return_to_here.call(call_args, 2, "rth", b());

      BasicBlock* ret_raise_val = new_block("ret_raise_val");
      bail_out_fast_ = new_block("ret_null");

      start->moveAfter(bail_out_fast_);

      b().CreateCondBr(isit, ret_raise_val, bail_out_fast_);

      set_block(bail_out_fast_);
      if(use_full_scope_) {
        flush_scope_to_heap(vars_);
      }

      info().add_return_value(Constant::getNullValue(ObjType), current_block());
      b().CreateBr(info().return_pad());

      set_block(ret_raise_val);
      Value* crv = f.clear_raise_value.call(&vm_, 1, "crv", b());
      if(use_full_scope_) flush_scope_to_heap(vars_);

      info().add_return_value(crv, current_block());
      b().CreateBr(info().return_pad());

      set_block(start);

      ip_pos_ = b().CreateConstGEP2_32(call_frame_, 0, offset::cf_ip, "ip_pos");

      global_serial_pos = b().CreateIntToPtr(
          ConstantInt::get(ls_->IntPtrTy, (intptr_t)ls_->shared().global_serial_address()),
          llvm::PointerType::getUnqual(ls_->IntPtrTy), "cast_to_intptr");

      init_out_args();

      vmm_debugging_ = constant(&info().vmm->debugging, llvm::PointerType::getUnqual(ls_->Int32Ty));
    }

    void set_has_side_effects() {
      has_side_effects_ = true;
    }

    bool has_side_effects() {
      return has_side_effects_;
    }

    void use_full_scope() {
      use_full_scope_ = true;
    }

    void set_called_args(int args) {
      called_args_ = args;
    }

    int sends_done() {
      return sends_done_;
    }

    Value* scope() {
      return b().CreateLoad(
          b().CreateConstGEP2_32(call_frame_, 0, offset::cf_scope, "scope_pos"),
          "scope");
    }

    Value* top_scope() {
      return b().CreateLoad(
          b().CreateConstGEP2_32(call_frame_, 0, offset::cf_top_scope, "top_scope_pos"),
          "top_scope");
    }

    BlockMap& block_map() {
      return block_map_;
    }

    bool has_exception_handler() {
      return current_jbb_->exception_handler != 0;
    }

    BasicBlock* exception_handler() {
      return current_jbb_->exception_handler->entry();
    }

    void check_for_return(Value* val) {
      BasicBlock* cont = new_block();

      Value* null = Constant::getNullValue(ObjType);

      BasicBlock* orig = current_block();

      Value* cmp = b().CreateICmpEQ(val, null, "null_check");
      BasicBlock* is_break = new_block("is_break");

      b().CreateCondBr(cmp, is_break, cont);

      /////
      set_block(is_break);

      Signature brk(ls_, ls_->Int1Ty);
      brk << VMTy;
      brk << CallFrameTy;

      Value* call_args[] = {
        vm_,
        call_frame_
      };

      Value* isit = brk.call("rbx_break_to_here", call_args, 2, "bth", b());

      BasicBlock* push_break_val = new_block("push_break_val");
      BasicBlock* next = 0;

      // If there are handlers...
      if(has_exception_handler()) {
        next = exception_handler();
      } else {
        next = bail_out_;
      }

      b().CreateCondBr(isit, push_break_val, next);

      ////
      set_block(push_break_val);

      Signature clear(ls_, ObjType);
      clear << VMTy;
      Value* crv = clear.call("rbx_clear_raise_value", &vm_, 1, "crv", b());

      b().CreateBr(cont);

      /////
      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, "possible_break");
      phi->addIncoming(val, orig);
      phi->addIncoming(crv, push_break_val);

      stack_push(phi);
    }

    void propagate_exception() {
      // If there are handlers...
      if(has_exception_handler()) {
        b().CreateBr(exception_handler());
      } else {
        b().CreateBr(bail_out_fast_);
      }
    }

    BasicBlock* check_for_exception_then(Value* val, BasicBlock* cont,
                                         bool pass_top=true)
    {
      Value* null = Constant::getNullValue(ObjType);

      BasicBlock* check_active = new_block("check_active");

      Value* is_exception = b().CreateICmpEQ(val, null, "null_check");

      // If there are handlers...
      if(has_exception_handler()) {
        b().CreateCondBr(is_exception, exception_handler(), check_active);
      } else {
        b().CreateCondBr(is_exception, bail_out_fast_, check_active);
      }

      set_block(check_active);

      if(!state()->config().jit_check_debugging) {
        b().CreateBr(cont);
        return check_active;
      }

      Value* is_debugging = b().CreateLoad(vmm_debugging_, "loaded_debugging_flag");

      BasicBlock* restart_in_interp = new_block("restart");

      // Check the active flag
      b().CreateCondBr(
          b().CreateICmpEQ(is_debugging, cint(1), "check_active"),
          restart_in_interp,
          cont);

      set_block(restart_in_interp);

      Value* sp = last_sp_as_int();

      flush();

      Signature sig(ls_, "Object");

      sig << "VM";
      sig << "CallFrame";
      sig << ls_->Int32Ty;
      sig << ls_->IntPtrTy;
      sig << "CallFrame";
      sig << ls_->Int32Ty;
      sig << llvm::PointerType::getUnqual(ls_->Int32Ty);

      int unwinds = emit_unwinds();

      Value* root_callframe = info().top_parent_call_frame();

      Value* call_args[] = {
        vm_,
        call_frame_,
        cint(next_ip_),
        sp,
        root_callframe,
        cint(unwinds),
        info().unwind_info(),
        (pass_top ? val : Constant::getNullValue(val->getType()))
      };

      Value* call = sig.call("rbx_continue_debugging", call_args, 8, "", b());

      info().add_return_value(call, current_block());
      b().CreateBr(info().return_pad());

      cont->moveAfter(restart_in_interp);

      return check_active;
    }

    void check_for_exception(Value* val, bool pass_top=true) {
      BasicBlock* cont = new_block();
      check_for_exception_then(val, cont, pass_top);
      set_block(cont);
    }

    void at_ip(int ip) {
      // Bad startup edge case
      if(ip == 0) {
        current_jbb_ = &(block_map_[0]);
        return;
      }

      BlockMap::iterator i = block_map_.find(ip);
      if(i != block_map_.end()) {
        JITBasicBlock& jbb = i->second;

        current_jbb_ = &jbb;

        if(BasicBlock* next = jbb.block) {
          if(!b().GetInsertBlock()->getTerminator()) {
            b().CreateBr(next);
          }

          next->moveAfter(b().GetInsertBlock());

          set_block(next);
        }
        if(jbb.sp != -10) set_sp(jbb.sp);
      }

      remember_sp();

      current_ip_ = ip;
    }

    void flush_ip() {
      b().CreateStore(ConstantInt::get(ls_->Int32Ty, current_ip_), ip_pos_);
    }

    void flush() {
      flush_ip();
      flush_stack();
    }

    /* push a global constant onto the stack */
    /* 0:cpath_top, 1:rubinius */
    void push_system_object(int which) {
      // we're calling something that returns an Object
      Signature sig(ls_, ObjType);
      // given a system state and a 32bit int
      sig << VMTy;
      sig << ls_->Int32Ty;

      // the actual values of which are the calling arguments
      Value* call_args[] = {
        vm_,
        ConstantInt::get(ls_->Int32Ty, which)
      };

      // call the function we just described using the builder
      Value* val = sig.call("rbx_push_system_object", call_args, 2, "so", b());
      stack_push(val);
    }

    // visitors.

    void visit(opcode op, opcode arg1, opcode arg2) {
      throw Unsupported();
    }

    void visit_pop() {
      stack_remove(1);
    }

    void visit_pop_many(opcode count) {
      stack_remove(count);
    }

    void visit_push_nil() {
      stack_push(constant(Qnil));
    }

    void visit_push_true() {
      stack_push(constant(Qtrue));
    }

    void visit_push_undef() {
      Object** addr = ls_->shared().globals.undefined.object_address();
      Value* l_addr = constant(addr, ObjArrayTy);

      stack_push(b().CreateLoad(l_addr, "undefined"));
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
      if(ls_->include_profiling() && method_entry_) {
        Value* test = b().CreateLoad(ls_->profiling(), "profiling");
        BasicBlock* end_profiling = new_block("end_profiling");
        BasicBlock* cont = new_block("continue");

        b().CreateCondBr(test, end_profiling, cont);

        set_block(end_profiling);

        Signature sig(ls_, ls_->VoidTy);
        sig << llvm::PointerType::getUnqual(ls_->Int8Ty);

        Value* call_args[] = {
          method_entry_
        };

        sig.call("rbx_end_profiling", call_args, 1, "", b());

        b().CreateBr(cont);

        set_block(cont);
      }

      if(use_full_scope_) flush_scope_to_heap(vars_);

      info().add_return_value(stack_top(), current_block());
      b().CreateBr(info().return_pad());
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

    void visit_dup_many(opcode count) {
      Value* pos = stack_objects(count);
      for(opcode i = 0; i < count; i++) {
        stack_push(b().CreateLoad(
              b().CreateConstGEP1_32(pos, i)));
      }
    }

    void visit_rotate(opcode count) {
      int diff = count / 2;

      for(int i = 0; i < diff; i++) {
        int offset = count - i - 1;
        Value* pos = stack_back_position(offset);
        Value* pos2 = stack_back_position(i);

        Value* val = b().CreateLoad(pos, "rotate");
        Value* val2 = b().CreateLoad(pos2, "rotate");

        b().CreateStore(val2, pos);
        b().CreateStore(val, pos2);
      }
    }

    void visit_move_down(opcode positions) {
      Value* val = stack_top();

      for(opcode i = 0; i < positions; i++) {
        int target = i;
        int current = target + 1;

        Value* tmp = stack_back(current);
        Value* pos = stack_back_position(target);

        b().CreateStore(tmp, pos);
      }

      b().CreateStore(val, stack_back_position(positions));
    }

    void visit_check_frozen() {
      Signature sig(ls_, "Object");

      sig << "VM";
      sig << "CallFrame";
      sig << "Object";

      Value* call_args[] = { vm_, call_frame_, stack_top() };

      Value* res = sig.call("rbx_check_frozen", call_args, 3, "", b());

      check_for_exception(res, false);
    }

    void check_fixnums(Value* left, Value* right, BasicBlock* if_true,
                       BasicBlock* if_false) {
      Value* mask = ConstantInt::get(ls_->IntPtrTy, TAG_FIXNUM_MASK);
      Value* tag  = ConstantInt::get(ls_->IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(left);
      Value* rint = cast_int(right);
      Value* both =   b().CreateAnd(lint, rint, "both");
      Value* masked = b().CreateAnd(both, mask, "masked");

      Value* cmp = b().CreateICmpEQ(masked, tag, "are_fixnums");

      b().CreateCondBr(cmp, if_true, if_false);
    }

    void check_both_not_references(Value* left, Value* right, BasicBlock* if_true,
                            BasicBlock* if_false) {
      Value* mask = ConstantInt::get(ls_->IntPtrTy, TAG_REF_MASK);
      Value* zero = ConstantInt::get(ls_->IntPtrTy, TAG_REF);

      Value* lint = cast_int(left);
      lint = b().CreateAnd(lint, mask, "mask");
      Value* lcmp = b().CreateICmpNE(lint, zero, "check_mask");

      BasicBlock* right_check = new_block("ref_check");
      right_check->moveAfter(current_block());
      b().CreateCondBr(lcmp, right_check, if_false);

      set_block(right_check);

      Value* rint = cast_int(right);
      rint = b().CreateAnd(rint, mask, "mask");
      Value* rcmp = b().CreateICmpNE(rint, zero, "check_mask");

      b().CreateCondBr(rcmp, if_true, if_false);
    }

    void add_send_args(Signature& sig) {
      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << ls_->IntPtrTy;
      sig << ObjArrayTy;
    }

    void setup_out_args(int args) {
      b().CreateStore(stack_back(args), out_args_recv_);
      b().CreateStore(constant(Qnil), out_args_block_);
      b().CreateStore(ConstantInt::get(ls_->Int32Ty, args),
                    out_args_total_);
      b().CreateStore(Constant::getNullValue(ptr_type("Tuple")),
                      out_args_container_);
      if(args > 0) {
        b().CreateStore(stack_objects(args), out_args_arguments_);
      }
    }

    void setup_out_args_with_block(int args) {
      b().CreateStore(stack_back(args + 1), out_args_recv_);
      b().CreateStore(stack_top(), out_args_block_);
      b().CreateStore(ConstantInt::get(ls_->Int32Ty, args),
                    out_args_total_);
      b().CreateStore(Constant::getNullValue(ptr_type("Tuple")),
                      out_args_container_);
      if(args > 0) {
        b().CreateStore(stack_objects(args + 1), out_args_arguments_);
      }
    }

    Value* invoke_inline_cache(InlineCache* cache) {
      Value* cache_const = b().CreateIntToPtr(
          ConstantInt::get(ls_->IntPtrTy, reinterpret_cast<uintptr_t>(cache)),
          ptr_type("InlineCache"), "cast_to_ptr");

      Value* execute_pos_idx[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::ic_execute),
      };

      Value* execute_pos = b().CreateGEP(cache_const,
          execute_pos_idx, execute_pos_idx+2, "execute_pos");

      Value* execute = b().CreateLoad(execute_pos, "execute");

      Value* call_args[] = {
        vm_,
        cache_const,
        call_frame_,
        out_args_
      };

      flush_ip();
      return b().CreateCall(execute, call_args, call_args+4, "ic_send");
    }

    Value* inline_cache_send(int args, InlineCache* cache) {
      sends_done_++;

      setup_out_args(args);
      return invoke_inline_cache(cache);
    }

    Value* block_send(InlineCache* cache, int args, bool priv=false) {
      sends_done_++;
      Value* cache_const = b().CreateIntToPtr(
          ConstantInt::get(ls_->IntPtrTy, reinterpret_cast<uintptr_t>(cache)),
          ptr_type("InlineCache"), "cast_to_ptr");

      Value* execute_pos_idx[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::ic_execute),
      };

      Value* execute_pos = b().CreateGEP(cache_const,
          execute_pos_idx, execute_pos_idx+2, "execute_pos");

      Value* execute = b().CreateLoad(execute_pos, "execute");

      setup_out_args_with_block(args);

      Value* call_args[] = {
        vm_,
        cache_const,
        call_frame_,
        out_args_
      };

      flush_ip();
      return b().CreateCall(execute, call_args, call_args+4, "ic_send");
    }

    Value* splat_send(Symbol* name, int args, bool priv=false) {
      sends_done_++;
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << ls_->IntPtrTy;
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
        constant(name),
        ConstantInt::get(ls_->IntPtrTy, args),
        stack_objects(args + 3),   // 3 == recv + block + splat
      };

      flush_ip();
      return sig.call(func_name, call_args, 5, "splat_send", b());
    }

    Value* super_send(Symbol* name, int args, bool splat=false) {
      Signature sig(ls_, ObjType);
      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << ls_->IntPtrTy;
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
        constant(name),
        ConstantInt::get(ls_->IntPtrTy, args),
        stack_objects(args + extra),
      };

      flush_ip();
      return sig.call(func_name, call_args, 5, "super_send", b());
    }

    void visit_meta_to_s(opcode name) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(name);
      set_has_side_effects();

      Value* recv = stack_top();

      Signature sig(ls_, "Object");
      sig << "VM";
      sig << "CallFrame";
      sig << "InlineCache";
      sig << "Object";

      Value* cache_const = b().CreateIntToPtr(
          ConstantInt::get(ls_->IntPtrTy, reinterpret_cast<uintptr_t>(cache)),
          ptr_type("InlineCache"), "cast_to_ptr");

      Value* args[] = {
        vm_,
        call_frame_,
        cache_const,
        recv
      };

      Value* ret = sig.call("rbx_meta_to_s", args, 4, "", b());
      stack_remove(1);
      check_for_exception(ret);
      stack_push(ret);
    }

    void visit_meta_send_op_equal(opcode name) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(name);
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block();

      check_both_not_references(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_cache_send(1, cache);
      BasicBlock* send_block =
        check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* cmp = b().CreateICmpEQ(recv, arg, "imm_cmp");
      Value* imm_value = b().CreateSelect(cmp,
          constant(Qtrue), constant(Qfalse), "select_bool");

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, "equal_value");
      phi->addIncoming(called_value, send_block);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_tequal(opcode name) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(name);
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block();

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_cache_send(1, cache);
      BasicBlock* send_block =
        check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* cmp = b().CreateICmpEQ(recv, arg, "imm_cmp");
      Value* imm_value = b().CreateSelect(cmp,
          constant(Qtrue), constant(Qfalse), "select_bool");

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, "equal_value");
      phi->addIncoming(called_value, send_block);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_lt(opcode name) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(name);
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_cache_send(1, cache);
      BasicBlock* send_bb = check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* cmp = b().CreateICmpSLT(recv, arg, "imm_cmp");
      Value* imm_value = b().CreateSelect(cmp,
          constant(Qtrue), constant(Qfalse), "select_bool");

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, "addition");
      phi->addIncoming(called_value, send_bb);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_gt(opcode name) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(name);
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_cache_send(1, cache);
      BasicBlock* send_bb = check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* cmp = b().CreateICmpSGT(recv, arg, "imm_cmp");
      Value* imm_value = b().CreateSelect(cmp,
          constant(Qtrue), constant(Qfalse), "select_bool");

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, "compare");
      phi->addIncoming(called_value, send_bb);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_plus(opcode name) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(name);
      set_has_side_effects();
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* tagnow = new_block("tagnow");
      BasicBlock* cont = new_block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_cache_send(1, cache);
      BasicBlock* send_bb = check_for_exception_then(called_value, cont);

      set_block(fast);

      std::vector<const Type*> types;
      types.push_back(FixnumTy);
      types.push_back(FixnumTy);

      std::vector<const Type*> struct_types;
      struct_types.push_back(FixnumTy);
      struct_types.push_back(ls_->Int1Ty);

      StructType* st = StructType::get(ls_->ctx(), struct_types);

      FunctionType* ft = FunctionType::get(st, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction(ADD_WITH_OVERFLOW, ft));

      Value* recv_int = tag_strip(recv);
      Value* arg_int = tag_strip(arg);
      Value* call_args[] = { recv_int, arg_int };
      Value* res = b().CreateCall(func, call_args, call_args+2, "add.overflow");

      Value* sum = b().CreateExtractValue(res, 0, "sum");
      Value* dof = b().CreateExtractValue(res, 1, "did_overflow");

      b().CreateCondBr(dof, dispatch, tagnow);

      set_block(tagnow);

      Value* imm_value = fixnum_tag(sum);

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, "addition");
      phi->addIncoming(called_value, send_bb);
      phi->addIncoming(imm_value, tagnow);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_minus(opcode name) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(name);
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_cache_send(1, cache);
      BasicBlock* send_bb = check_for_exception_then(called_value, cont);

      set_block(fast);

      std::vector<const Type*> types;
      types.push_back(FixnumTy);
      types.push_back(FixnumTy);

      std::vector<const Type*> struct_types;
      struct_types.push_back(FixnumTy);
      struct_types.push_back(ls_->Int1Ty);

      StructType* st = StructType::get(ls_->ctx(), struct_types);

      FunctionType* ft = FunctionType::get(st, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction(SUB_WITH_OVERFLOW, ft));

      Value* recv_int = tag_strip(recv);
      Value* arg_int = tag_strip(arg);
      Value* call_args[] = { recv_int, arg_int };
      Value* res = b().CreateCall(func, call_args, call_args+2, "sub.overflow");

      Value* sum = b().CreateExtractValue(res, 0, "sub");
      Value* dof = b().CreateExtractValue(res, 1, "did_overflow");

      BasicBlock* tagnow = new_block("tagnow");

      b().CreateCondBr(dof, dispatch, tagnow);

      set_block(tagnow);
      Value* imm_value = fixnum_tag(sum);

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, "subtraction");
      phi->addIncoming(called_value, send_bb);
      phi->addIncoming(imm_value, tagnow);

      stack_remove(2);
      stack_push(phi);
    }

    Object* literal(opcode which) {
      return info().method()->literals()->at(which);
    }

    Value* get_literal(opcode which) {
      Value* gep = b().CreateConstGEP2_32(call_frame_, 0, offset::cf_cm, "cm_pos");
      Value* cm =  b().CreateLoad(gep, "cm");

      gep = b().CreateConstGEP2_32(cm, 0, offset::cm_literals, "literals_pos");
      Value* lits = b().CreateLoad(gep, "literals");

      Value* idx2[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::tuple_field),
        ConstantInt::get(ls_->Int32Ty, which)
      };

      gep = b().CreateGEP(lits, idx2, idx2+3, "literal_pos");
      return b().CreateLoad(gep, "literal");
    }

    void visit_push_literal(opcode which) {
      Object* lit = literal(which);
      if(Symbol* sym = try_as<Symbol>(lit)) {
        stack_push(constant(sym));
      } else if(kind_of<Fixnum>(lit)) {
        stack_push(constant(lit));
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

      Value* dup = b().CreateCall(func, call_args, call_args+3, "string_dup");
      check_for_exception(dup);
      stack_push(dup);
    }

    void push_scope_local(Value* scope, opcode which) {
      Value* pos = b().CreateConstGEP2_32(scope, 0, offset::varscope_locals,
                                     "locals_pos");

      Value* table = b().CreateLoad(pos, "locals");

      Value* val_pos = b().CreateConstGEP1_32(table, which, "local_pos");

      stack_push(b().CreateLoad(val_pos, "local"));
    }

    Value* local_location(Value* vars, opcode which) {
      Value* idx2[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        ConstantInt::get(ls_->Int32Ty, which)
      };

      return b().CreateGEP(vars, idx2, idx2+3, "local_pos");
    }

    void visit_push_stack_local(opcode which) {
      Value* pos = stack_slot_position(vmmethod()->stack_size - which - 1);
      stack_push(b().CreateLoad(pos, "stack_local"));
    }

    void visit_set_stack_local(opcode which) {
      Value* pos = stack_slot_position(vmmethod()->stack_size - which - 1);
      b().CreateStore(stack_top(), pos);
    }

    void visit_push_local(opcode which) {
      Value* idx2[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        ConstantInt::get(ls_->Int32Ty, which)
      };

      Value* pos = b().CreateGEP(vars_, idx2, idx2+3, "local_pos");

      stack_push(b().CreateLoad(pos, "local"));
    }

    void set_scope_local(Value* scope, opcode which) {
      Value* pos = b().CreateConstGEP2_32(scope, 0, offset::varscope_locals,
                                     "locals_pos");

      Value* table = b().CreateLoad(pos, "locals");

      Value* val_pos = b().CreateConstGEP1_32(table, which, "local_pos");

      Value* val = stack_top();

      b().CreateStore(val, val_pos);

      write_barrier(scope, val);
    }

    void visit_set_local(opcode which) {
      Value* idx2[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_tuple),
        ConstantInt::get(ls_->Int32Ty, which)
      };

      Value* pos = b().CreateGEP(vars_, idx2, idx2+3, "local_pos");

      Value* val;

      JITStackArgs* inline_args = incoming_args();
      if(inline_args && current_hint() == cHintLazyBlockArgs) {
        std::vector<const Type*> types;
        types.push_back(ls_->ptr_type("VM"));
        types.push_back(ls_->Int32Ty);

        FunctionType* ft = FunctionType::get(ls_->ptr_type("Object"), types, true);
        Function* func = cast<Function>(
            ls_->module()->getOrInsertFunction("rbx_create_array", ft));

        std::vector<Value*> outgoing_args;
        outgoing_args.push_back(vm());

        int ary_size;
        if(block_arg_shift_ >= (int)inline_args->size()) {
          ary_size = 0;
        } else {
          ary_size = (int)inline_args->size() - block_arg_shift_;
        }

        outgoing_args.push_back(ConstantInt::get(ls_->Int32Ty, ary_size));

        if(ary_size > 0) {
          for(size_t i = block_arg_shift_; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }
        }

        val = b().CreateCall(func, outgoing_args.begin(), outgoing_args.end(), "ary");
      } else {
        val = stack_top();
      }

      b().CreateStore(val, pos);
    }

    Value* get_self(Value* vars = 0) {
      if(!vars) vars = vars_;

      assert(vars);

      return b().CreateLoad(
          b().CreateConstGEP2_32(vars, 0, offset::vars_self),
          "self");
    }

    Value* get_block() {
      return b().CreateLoad(
          b().CreateConstGEP2_32(vars_, 0, offset::vars_block, "block_pos"));
    }

    void visit_push_self() {
      stack_push(get_self());
    }

    void visit_allow_private() {
      allow_private_ = true;
    }

    void novisit_set_call_flags(opcode flag) {
      call_flags_ = flag;
    }

    int emit_unwinds() {
      int count = 0;

      JITBasicBlock* jbb = current_jbb_;

      if(!jbb->exception_handler) return 0;

      while(jbb->exception_handler) {
        count++;
        jbb = jbb->exception_handler;
      }

      jbb = current_jbb_->exception_handler;
      Value* unwinds = info().unwind_info();

      for(int slice = (count - 1) * 3; slice >= 0; slice -= 3) {
        /*
        std::cout << "unwind: " << jbb->start_ip
                  << " sp: " << jbb->sp
                  << " type: " << jbb->exception_type
                  << "\n";
        */

        b().CreateStore(
            cint(jbb->start_ip),
            b().CreateConstGEP1_32(unwinds, slice, "unwind_ip"));
        b().CreateStore(
            cint(jbb->sp),
            b().CreateConstGEP1_32(unwinds, slice + 1, "unwind_sp"));
        b().CreateStore(
            cint(jbb->exception_type),
            b().CreateConstGEP1_32(unwinds, slice + 2, "unwind_type"));

        jbb = jbb->exception_handler;
      }

      return count;
    }

    void emit_uncommon() {
      emit_delayed_create_block(true);

      Value* sp = last_sp_as_int();

      flush();

      Signature sig(ls_, "Object");

      sig << "VM";
      sig << "CallFrame";
      sig << ls_->Int32Ty;
      sig << ls_->IntPtrTy;
      sig << "CallFrame";
      sig << ls_->Int32Ty;
      sig << llvm::PointerType::getUnqual(ls_->Int32Ty);

      int unwinds = emit_unwinds();

      Value* root_callframe = info().top_parent_call_frame();

      Value* call_args[] = {
        vm_,
        call_frame_,
        cint(current_ip_),
        sp,
        root_callframe,
        cint(unwinds),
        info().unwind_info()
      };

      Value* call = sig.call("rbx_continue_uncommon", call_args, 7, "", b());

      info().add_return_value(call, current_block());
      b().CreateBr(info().return_pad());
    }

    void visit_invoke_primitive(opcode which, opcode args) {
      InvokePrimitive invoker = reinterpret_cast<InvokePrimitive>(which);

      BasicBlock* fin = 0;
      Value* inline_klass = 0;
      BasicBlock* inline_body = 0;

      if(invoker == invoke_object_class && args == 1) {
        if(state()->config().jit_inline_debug) {
          context().inline_log("inlining") << "custom object_class invoker\n";
        }
        Value* obj = stack_back(0);
        Value* is_ref = check_is_reference(obj);

        BasicBlock* cont = new_block("check_class");
        BasicBlock* failure = new_block("use_call");
        inline_body = new_block("found_class");
        fin = new_block("got_class");

        b().CreateCondBr(is_ref, cont, failure);

        set_block(cont);

        inline_klass = b().CreateBitCast(reference_class(obj), ObjType);

        Value* is_class = check_type_bits(inline_klass, rubinius::Class::type, "is_class");

        b().CreateCondBr(is_class, inline_body, failure);

        set_block(inline_body);

        b().CreateBr(fin);

        set_block(failure);
        failure->moveAfter(inline_body);
      }

      Signature sig(ls_, "Object");
      sig << "VM";
      sig << "CallFrame";
      sig << ObjArrayTy;
      sig << ls_->Int32Ty;

      Value* arg_ary = stack_objects(args);

      Value* call_args[] = { vm_, call_frame_, arg_ary, cint(args) };

      Value* ptr = b().CreateIntToPtr(
          ConstantInt::get(ls_->IntPtrTy, reinterpret_cast<uintptr_t>(invoker)),
          llvm::PointerType::getUnqual(sig.type()));

      Value* call = b().CreateCall(ptr, call_args, call_args + 4, "invoked_prim");
      stack_remove(args);

      check_for_exception(call);

      if(fin) {
        BasicBlock* cur = current_block();

        b().CreateBr(fin);

        set_block(fin);

        PHINode* phi = b().CreatePHI(ObjType, "object_class");
        phi->addIncoming(inline_klass, inline_body);
        phi->addIncoming(call, cur);

        stack_push(phi);
      } else {
        stack_push(call);
      }
    }

    void invoke_inline_cache(InlineCache* cache, opcode args) {
      set_has_side_effects();

      Value* ret = inline_cache_send(args, cache);
      stack_remove(args + 1);
      check_for_exception(ret);
      stack_push(ret);

      allow_private_ = false;
    }

    void visit_send_stack(opcode which, opcode args) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(which);

      if(cache->method) {
        BasicBlock* failure = new_block("fallback");
        BasicBlock* cont = new_block("continue");

        Inliner inl(context(), *this, cache, args, failure);
        if(inl.consider()) {
          // Uncommon doesn't yet know how to synthesize UnwindInfos, so
          // don't do uncommon if there are handlers.
          if(!inl.fail_to_send() && !in_inlined_block()) {
            BasicBlock* cur = b().GetInsertBlock();

            set_block(failure);
            emit_uncommon();

            set_block(cur);
            stack_remove(args+1);
            if(inl.check_for_exception()) {
              check_for_exception(inl.result());
            }
            stack_push(inl.result());

            b().CreateBr(cont);

            set_block(cont);
          } else {
            // Emit both the inlined code and a send for it
            BasicBlock* inline_block = b().GetInsertBlock();

            b().CreateBr(cont);

            set_block(failure);
            Value* send_res = inline_cache_send(args, cache);
            b().CreateBr(cont);

            set_block(cont);
            PHINode* phi = b().CreatePHI(ObjType, "send_result");
            phi->addIncoming(inl.result(), inline_block);
            phi->addIncoming(send_res, failure);

            stack_remove(args + 1);
            check_for_exception(phi);

            stack_push(phi);
          }

          allow_private_ = false;
          return;
        }
      }

      set_has_side_effects();

      Value* ret = inline_cache_send(args, cache);
      stack_remove(args + 1);
      check_for_exception(ret);
      stack_push(ret);

      allow_private_ = false;
    }

    void visit_call_custom(opcode which, opcode args) {
      if(state()->config().jit_inline_debug) {
        ls_->log() << "generate: call_custom\n";
      }

      InlineCache* cache = reinterpret_cast<InlineCache*>(which);

      set_has_side_effects();

      Value* ret = inline_cache_send(args, cache);
      stack_remove(args + 1);
      check_for_exception(ret);
      stack_push(ret);

      allow_private_ = false;
    }

    void visit_send_method(opcode which) {
      visit_send_stack(which, 0);
    }

    bool in_inlined_block() {
      return for_inlined_method() && info().is_block;
    }

    void emit_delayed_create_block(bool always=false) {
      std::list<JITMethodInfo*> in_scope;
      JITMethodInfo* nfo = &info();

      while(nfo) {
        in_scope.push_front(nfo);
        nfo = nfo->parent_info();
      }

      for(std::list<JITMethodInfo*>::iterator i = in_scope.begin();
          i != in_scope.end();
          i++ ) {
        nfo = *i;

        JITInlineBlock* ib = nfo->inline_block();
        if(ib && ib->code() && (always || !ib->created_object_p())) {
          JITMethodInfo* creator = ib->creation_scope();
          assert(creator);

          Signature sig(ls_, ObjType);
          sig << VMTy;
          sig << CallFrameTy;
          sig << ls_->Int32Ty;

          Value* call_args[] = {
            vm_,
            creator->call_frame(),
            ConstantInt::get(ls_->Int32Ty, ib->which())
          };

          Value* blk = sig.call("rbx_create_block", call_args, 3,
              "delayed_create_block", b());

          b().CreateStore(
              blk,
              b().CreateConstGEP2_32(nfo->variables(), 0,
                offset::vars_block),
              false);

          if(!always) ib->set_created_object();
        }
      }
    }

    void emit_create_block(opcode which, bool push=false) {
      // if we're inside an inlined method that has a block
      // visible, that means that we've note yet emited the code to
      // actually create the block for this inlined block.
      //
      // But, because we're about to create a block here, it might
      // want to yield (ie, inlining Enumerable#find on an Array, but
      // not inlining the call to each inside find).
      //
      // So at this point, we have to create the block object
      // for this parent block.

      emit_delayed_create_block();

      std::vector<const Type*> types;
      types.push_back(VMTy);

      // we use stack_set_top here because we always have a placeholder
      // on the stack that we're going to just replace.

      if(in_inlined_block()) {
        types.push_back(ObjType);
        types.push_back(ls_->Int32Ty);
        types.push_back(ls_->Int32Ty);

        FunctionType* ft = FunctionType::get(ObjType, types, true);
        Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_create_block_multi", ft));

        std::vector<Value*> call_args;
        call_args.push_back(vm_);
        call_args.push_back(get_literal(which));
        call_args.push_back(ConstantInt::get(ls_->Int32Ty, which));

        std::vector<JITMethodInfo*> mis;

        JITMethodInfo* nfo = &info();
        while(nfo) {
          mis.push_back(nfo);
          nfo = nfo->creator_info();
        }

        call_args.push_back(ConstantInt::get(ls_->Int32Ty, mis.size()));

        for(std::vector<JITMethodInfo*>::reverse_iterator i = mis.rbegin();
            i != mis.rend();
            ++i) {
          call_args.push_back((*i)->call_frame());
        }

        if(push) {
          stack_push(b().CreateCall(func, call_args.begin(), call_args.end(), "create_block"));
        } else {
          stack_set_top(b().CreateCall(func, call_args.begin(), call_args.end(), "create_block"));
        }
        return;
      };

      types.push_back(CallFrameTy);
      types.push_back(ls_->Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_create_block", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(ls_->Int32Ty, which)
      };

      if(push) {
        stack_push(b().CreateCall(func, call_args, call_args+3, "create_block"));
      } else {
        stack_set_top(b().CreateCall(func, call_args, call_args+3, "create_block"));
      }
    }

    void visit_create_block(opcode which) {
      // If we're creating a block to pass directly to
      // send_stack_with_block, delay doing so.
      if(next_op() == InstructionSequence::insn_send_stack_with_block) {
        // Push a placeholder and register which literal we would
        // use for the block. The later send handles whether to actually
        // emit the call to create the block (replacing the placeholder)
        stack_push(constant(Qnil));
        current_block_ = (int)which;
      } else {
        current_block_ = -1;
        emit_create_block(which, true);
      }
    }

    void visit_send_stack_with_block(opcode which, opcode args) {
      set_has_side_effects();

      bool has_literal_block = (current_block_ >= 0);
      bool block_on_stack = !has_literal_block;

      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
      CompiledMethod* block_code = 0;

      if(cache->method &&
          ls_->config().jit_inline_blocks &&
          !context().inlined_block()) {
        if(has_literal_block) {
          block_code = try_as<CompiledMethod>(literal(current_block_));

          // Run the policy on the block code here, if we're not going to
          // inline it, don't inline this either.
          InlineOptions opts;
          opts.inlining_block();

          InlineDecision decision = inline_policy()->inline_p(
                                      block_code->backend_method(), opts);
          if(decision == cTooComplex) {
            if(state()->config().jit_inline_debug) {
              context().inline_log("NOT inlining")
                        << "block was too complex\n";
            }
            goto use_send;
          } else if(decision != cInline) {
            goto use_send;
          }
        } else {
          // If there is no literal block, we don't inline the method at all.
          // This is probably overkill, we should revise this and inline
          // the method anyway.
          goto use_send;
        }

        // Ok, we decision was cInline, so lets do this!
        BasicBlock* failure = new_block("fallback");
        BasicBlock* cont = new_block("continue");
        BasicBlock* cleanup = new_block("send_done");
        PHINode* send_result = b().CreatePHI(ObjType, "send_result");

        Inliner inl(context(), *this, cache, args, failure);

        VMMethod* code = 0;
        if(block_code) code = block_code->backend_method();
        JITInlineBlock block_info(ls_, send_result, cleanup, block_code, code, &info(),
                                  current_block_);

        inl.set_inline_block(&block_info);

        int stack_cleanup = args + 2;

        // So that the inliner can find recv and args properly.
        inl.set_block_on_stack();

        if(inl.consider()) {
          if(!inl.fail_to_send() && !in_inlined_block()) {
            send_result->addIncoming(inl.result(), b().GetInsertBlock());

            b().CreateBr(cleanup);

            set_block(failure);
            if(!block_on_stack) {
              emit_create_block(current_block_);
              block_on_stack = true;
            }
            emit_uncommon();

            set_block(cleanup);
            send_result->removeFromParent();
            cleanup->getInstList().push_back(send_result);

            // send_result->moveBefore(&cleanup->back());

            stack_remove(stack_cleanup);
            if(inl.check_for_exception()) {
              check_for_exception(send_result);
            }

            stack_push(send_result);

            b().CreateBr(cont);

            set_block(cont);
          } else {
            // Emit both the inlined code and a send for it
            send_result->addIncoming(inl.result(), b().GetInsertBlock());

            b().CreateBr(cleanup);

            set_block(failure);
            if(!block_on_stack) {
              emit_create_block(current_block_);
              block_on_stack = true;
            }

            Value* send_res = block_send(cache, args, allow_private_);
            b().CreateBr(cleanup);

            set_block(cleanup);
            send_result->removeFromParent();
            cleanup->getInstList().push_back(send_result);

            send_result->addIncoming(send_res, failure);

            stack_remove(stack_cleanup);
            check_for_exception(send_result);

            stack_push(send_result);
          }

          allow_private_ = false;

          // Clear the current block
          current_block_ = -1;
          return;
        }

        // Don't need it.
        send_result->eraseFromParent();
      }

use_send:

      // Detect a literal block being created and passed here.
      if(!block_on_stack) {
        emit_create_block(current_block_);
      }

      Value* ret = block_send(cache, args, allow_private_);
      stack_remove(args + 2);
      check_for_return(ret);
      allow_private_ = false;

      // Clear the current block
      current_block_ = -1;
    }

    void visit_send_stack_with_splat(opcode which, opcode args) {
      set_has_side_effects();

      if(current_block_ >= 0) {
        emit_create_block(current_block_);
      }

      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
      Value* ret = splat_send(cache->name, args, allow_private_);
      stack_remove(args + 3);
      check_for_exception(ret);
      stack_push(ret);
      allow_private_ = false;

      // Clear the current block
      current_block_ = -1;
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

      Value* val = b().CreateCall(func, call_args, call_args+3, "cast_array");
      check_for_exception(val);
      stack_push(val);
    }

    void visit_cast_multi_value() {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_cast_multi_value", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_pop()
      };

      Value* val = b().CreateCall(func, call_args, call_args+3, "cast_multi_value");
      check_for_exception(val);
      stack_push(val);
    }

    void visit_push_block() {
      stack_push(get_block());
    }

    void visit_push_has_block() {
      // We're in an inlined method, we know if there is a block staticly.
      if(info().for_inlined_method()) {
        // Check for an inlined block being present, if so, this is a
        // constant!
        //
        JITInlineBlock* ib = info().inline_block();

        if(ib && ib->code()) {
          stack_push(constant(Qtrue));
        } else {
          stack_push(constant(Qfalse));
        }

        return;
      }

      Value* cmp = b().CreateICmpNE(get_block(), constant(Qnil), "is_nil");
      Value* imm_value = b().CreateSelect(cmp, constant(Qtrue),
          constant(Qfalse), "select_bool");
      stack_push(imm_value);
    }

    void visit_push_proc() {
      set_has_side_effects();

      Signature sig(ls_, ObjType);
      sig << "VM";
      sig << "CallFrame";

      Value* args[] = {
        vm_,
        call_frame_
      };

      Value* prc = sig.call("rbx_make_proc", args, 2, "proc", b());
      check_for_exception(prc);
      stack_push(prc);
    }

    void visit_send_super_stack_with_block(opcode which, opcode args) {
      set_has_side_effects();

      if(current_block_ >= 0) {
        emit_create_block(current_block_);
      }

      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
      Value* ret = super_send(cache->name, args);
      stack_remove(args + 1);
      check_for_return(ret);

      current_block_ = -1;
    }

    void visit_send_super_stack_with_splat(opcode which, opcode args) {
      set_has_side_effects();

      if(current_block_ >= 0) {
        emit_create_block(current_block_);
      }

      InlineCache* cache = reinterpret_cast<InlineCache*>(which);
      Value* ret = super_send(cache->name, args, true);
      stack_remove(args + 2);
      check_for_exception(ret);
      stack_push(ret);

      current_block_ = -1;
    }

    void visit_zsuper(opcode which) {
      set_has_side_effects();

      if(current_block_ >= 0) {
        emit_create_block(current_block_);
      }

      InlineCache* cache = reinterpret_cast<InlineCache*>(which);

      Signature sig(ls_, ObjType);
      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(cache->name),
        stack_top()
      };

      flush_ip();
      Value* ret = sig.call("rbx_zsuper_send", call_args, 4, "super_send", b());
      stack_remove(1);

      check_for_exception(ret);
      stack_push(ret);

      current_block_ = -1;
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

      b().CreateCall(func, call_args, call_args+3);
    }

    Object* current_literal(opcode which) {
      return info().method()->literals()->at(which);
    }

    void visit_push_const_fast(opcode name, opcode cache) {
      set_has_side_effects();

      BasicBlock* cont = 0;

      Value* cached_value = 0;
      BasicBlock* cached_block = 0;

      GlobalCacheEntry* entry = try_as<GlobalCacheEntry>(current_literal(cache));
      if(entry) {
        assert(entry->pin());

        Value* global_serial = b().CreateLoad(global_serial_pos, "global_serial");

        Value* current_serial_pos = b().CreateIntToPtr(
            ConstantInt::get(ls_->IntPtrTy, (intptr_t)entry->serial_location()),
            llvm::PointerType::getUnqual(ls_->IntPtrTy), "cast_to_intptr");

        Value* current_serial = b().CreateLoad(current_serial_pos, "serial");

        Value* cmp = b().CreateICmpEQ(global_serial, current_serial, "use_cache");

        BasicBlock* use_cache = new_block("use_cache");
        BasicBlock* use_call  = new_block("use_call");
        cont =      new_block("continue");

        b().CreateCondBr(cmp, use_cache, use_call);

        set_block(use_cache);

        Value* value_pos = b().CreateIntToPtr(
            ConstantInt::get(ls_->IntPtrTy, (intptr_t)entry->value_location()),
            llvm::PointerType::getUnqual(ObjType), "cast_to_objptr");

        cached_value = b().CreateLoad(value_pos, "cached_value");
        cached_block = b().GetInsertBlock();

        b().CreateBr(cont);

        set_block(use_call);
      }

      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(ls_->Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_const_fast", ft));

      func->setOnlyReadsMemory(true);
      func->setDoesNotThrow(true);

      flush();

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(as<Symbol>(literal(name))),
        ConstantInt::get(ls_->Int32Ty, cache)
      };

      CallInst* ret = b().CreateCall(func, call_args, call_args+4,
                                       "push_const_fast");

      ret->setOnlyReadsMemory(true);
      ret->setDoesNotThrow(true);

      check_for_exception(ret);

      if(entry) {
        BasicBlock* ret_block = b().GetInsertBlock();
        b().CreateBr(cont);
        set_block(cont);

        PHINode* phi = b().CreatePHI(ObjType, "constant");
        phi->addIncoming(cached_value, cached_block);
        phi->addIncoming(ret, ret_block);

        stack_push(phi);
      } else {
        stack_push(ret);
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

      flush();

      Value* call_args[] = {
        vm_,
        call_frame_,
        constant(as<Symbol>(literal(name)))
      };

      Value* ret = b().CreateCall(func, call_args, call_args+3, "push_const_fast");
      check_for_exception(ret);
      stack_push(ret);
    }

    void visit_set_const(opcode name) {
      set_has_side_effects();

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

      b().CreateCall(func, call_args, call_args+4);
    }

    void visit_set_const_at(opcode name) {
      set_has_side_effects();

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

      b().CreateCall(func, call_args, call_args+4);

      stack_push(val);
    }

    void visit_set_literal(opcode which) {
      set_has_side_effects();

      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ls_->Int32Ty);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_literal", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(ls_->Int32Ty, which),
        stack_top()
      };

      b().CreateCall(func, call_args, call_args+4);
    }

    void visit_push_variables() {
      set_has_side_effects();

      Signature sig(ls_, ObjType);
      sig << "VM";
      sig << "CallFrame";

      Value* args[] = {
        vm_,
        call_frame_
      };

      stack_push(sig.call("rbx_promote_variables", args, 2, "promo_vars", b()));
    }

    void visit_push_scope() {
      if(info().is_block && !in_inlined_block()) {
        Value* scope = b().CreateLoad(
            b().CreateConstGEP2_32(call_frame_, 0, offset::cf_static_scope, "scope_pos"),
            "cm");

        stack_push(scope);
      } else {
        Value* cm;

        if(in_inlined_block()) {
          JITMethodInfo* creator = info().creator_info();
          assert(creator);
          cm = b().CreateLoad(
              b().CreateConstGEP2_32(creator->call_frame(), 0, offset::cf_cm, "cm_pos"),
              "cm");
        } else {
          cm = b().CreateLoad(
              b().CreateConstGEP2_32(call_frame_, 0, offset::cf_cm, "cm_pos"),
              "cm");
        }

        Value* gep = b().CreateConstGEP2_32(cm, 0, offset::cm_static_scope, "scope_pos");
        stack_push(b().CreateLoad(gep, "scope"));
      }
    }

    void visit_cast_for_single_block_arg() {
      JITStackArgs* inline_args = incoming_args();
      if(inline_args) {
        switch(inline_args->size()) {
        case 0:
          stack_push(constant(Qnil));
          break;
        case 1:
          stack_push(inline_args->at(0));
          break;
        default: {
          std::vector<const Type*> types;
          types.push_back(ls_->ptr_type("VM"));
          types.push_back(ls_->Int32Ty);

          FunctionType* ft = FunctionType::get(ls_->ptr_type("Object"), types, true);
          Function* func = cast<Function>(
              ls_->module()->getOrInsertFunction("rbx_create_array", ft));

          std::vector<Value*> outgoing_args;
          outgoing_args.push_back(vm());
          outgoing_args.push_back(ConstantInt::get(ls_->Int32Ty, inline_args->size()));

          for(size_t i = 0; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }

          Value* ary =
            b().CreateCall(func, outgoing_args.begin(), outgoing_args.end(), "ary");
          stack_push(ary);
         }
        }
      } else {
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

        stack_push(b().CreateCall(func, call_args, call_args+2, "cfsba"));
      }
    }

    void visit_cast_for_multi_block_arg() {
      JITStackArgs* inline_args = incoming_args();
      if(inline_args) {
        if(inline_args->size() == 1) {
          std::vector<const Type*> types;
          types.push_back(ls_->ptr_type("VM"));
          types.push_back(CallFrameTy);
          types.push_back(ls_->Int32Ty);

          FunctionType* ft = FunctionType::get(ls_->ptr_type("Object"), types, true);
          Function* func = cast<Function>(
              ls_->module()->getOrInsertFunction("rbx_cast_for_multi_block_arg_varargs", ft));

          std::vector<Value*> outgoing_args;
          outgoing_args.push_back(vm());
          outgoing_args.push_back(call_frame_);
          outgoing_args.push_back(ConstantInt::get(ls_->Int32Ty, inline_args->size()));

          for(size_t i = 0; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }

          Value* ary =
            b().CreateCall(func, outgoing_args.begin(), outgoing_args.end(), "ary");
          check_for_exception(ary);
          stack_push(ary);
        } else {
          stack_push(constant(Qundef)); // holder
          set_hint(cHintLazyBlockArgs);
        }
      } else {
        Signature sig(ls_, ObjType);
        sig << VMTy;
        sig << CallFrameTy;
        sig << ptr_type("Arguments");

        Value* call_args[] = {
          vm_,
          call_frame_,
          args_
        };

        Value* val = sig.call("rbx_cast_for_multi_block_arg", call_args, 3,
            "cfmba", b());
        check_for_exception(val);
        stack_push(val);
      }
    }

    void visit_cast_for_splat_block_arg() {
      JITStackArgs* inline_args = incoming_args();
      if(inline_args) {
        std::vector<const Type*> types;
        types.push_back(ls_->ptr_type("VM"));
        types.push_back(ls_->Int32Ty);

        FunctionType* ft = FunctionType::get(ls_->ptr_type("Object"), types, true);
        Function* func = cast<Function>(
            ls_->module()->getOrInsertFunction("rbx_create_array", ft));

        // If the arguments came from an unboxed array, we have to put them
        // back in the array before splatting them.
        if(inline_args->from_unboxed_array()) {
          std::vector<Value*> outgoing_args;
          outgoing_args.push_back(vm());
          outgoing_args.push_back(ConstantInt::get(ls_->Int32Ty, inline_args->size()));

          for(size_t i = 0; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }

          Value* ary =
            b().CreateCall(func, outgoing_args.begin(), outgoing_args.end(), "ary");

          Value* outargs2[] = {
            vm(),
            ConstantInt::get(ls_->Int32Ty, 1),
            ary
          };

          Value* wrapped = b().CreateCall(func, outargs2, outargs2 + 3, "splat_ary");
          stack_push(wrapped);
        } else {
          std::vector<Value*> outgoing_args;
          outgoing_args.push_back(vm());
          outgoing_args.push_back(ConstantInt::get(ls_->Int32Ty, inline_args->size()));

          for(size_t i = 0; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }

          Value* ary =
            b().CreateCall(func, outgoing_args.begin(), outgoing_args.end(), "ary");
          stack_push(ary);
        }
      } else {
        Signature sig(ls_, ObjType);
        sig << VMTy;
        sig << ptr_type("Arguments");

        Value* call_args[] = {
          vm_,
          args_
        };

        Value* val = sig.call("rbx_cast_for_splat_block_arg", call_args, 2,
            "cfmba", b());
        stack_push(val);
      }
    }

    void visit_set_local_depth(opcode depth, opcode index) {
      set_has_side_effects();

      // We're inlinig a block...
      if(in_inlined_block()) {
        JITMethodInfo* nfo = upscope_info(depth);

        if(nfo) {
          Value* local_pos = local_location(nfo->variables(), index);
          b().CreateStore(stack_top(), local_pos);
        } else {
          JITMethodInfo* nfo = &info();

          while(nfo->creator_info()) {
            nfo = nfo->creator_info();
            depth--;
          }

          // Now we've got as far up as we can see, so we'll search like
          // normal up from here.

          assert(nfo->is_block && "confused, top must be a block");

          /*
          Value* idx[] = {
            ConstantInt::get(ls_->Int32Ty, 0),
            ConstantInt::get(ls_->Int32Ty, offset::vars_parent)
          };

          Value* varscope = b().CreateLoad(
              b().CreateGEP(vars_, idx, idx+2), "scope.parent");
          */

          Signature sig(ls_, ObjType);
          sig << VMTy;
          sig << "CallFrame";
          sig << ObjType;
          sig << ls_->Int32Ty;
          sig << ls_->Int32Ty;

          Value* call_args[] = {
            vm_,
            nfo->call_frame(),
            stack_top(),
            ConstantInt::get(ls_->Int32Ty, depth),
            ConstantInt::get(ls_->Int32Ty, index)
          };

          Value* val = sig.call("rbx_set_local_from", call_args, 5, "vs_uplocal", b());
          check_for_exception(val);
        }
        return;
      }

      if(depth == 0) {
        visit_set_local(index);
        return;
      }
      /*
      else if(depth == 1) {
        Value* idx[] = {
          ConstantInt::get(ls_->Int32Ty, 0),
          ConstantInt::get(ls_->Int32Ty, offset::vars_parent)
        };

        Value* gep = b().CreateGEP(vars_, idx, idx+2, "parent_pos");

        Value* parent = b().CreateLoad(gep, "scope.parent");
        set_scope_local(parent, index);
        return;
      }
      */

      // Handle depth > 1

      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(ls_->Int32Ty);
      types.push_back(ls_->Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_local_depth", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_pop(),
        ConstantInt::get(ls_->Int32Ty, depth),
        ConstantInt::get(ls_->Int32Ty, index)
      };

      Value* val = b().CreateCall(func, call_args, call_args+5, "sld");
      check_for_exception(val);
      stack_push(val);
    }

    JITMethodInfo* upscope_info(int which) {
      JITMethodInfo* nfo = &info();

      for(int i = 0; i < which; i++) {
        nfo = nfo->creator_info();
        if(!nfo) return 0;
      }

      return nfo;
    }

    void visit_push_local_depth(opcode depth, opcode index) {
      set_has_side_effects();

      // We're in an inlined block..
      if(in_inlined_block()) {
        JITMethodInfo* nfo = upscope_info(depth);

        // And we can see this scope depth directly because of inlining...
        if(nfo) {
          Value* local_pos = local_location(nfo->variables(), index);
          stack_push(b().CreateLoad(local_pos, "upscope_local"));
        } else {
          JITMethodInfo* nfo = &info();

          while(nfo->creator_info()) {
            nfo = nfo->creator_info();
            depth--;
          }

          // Now we've got as far up as we can see, so we'll search like
          // normal up from here.

          assert(nfo->is_block && "confused, top must be a block");

          /*
          Value* idx[] = {
            ConstantInt::get(ls_->Int32Ty, 0),
            ConstantInt::get(ls_->Int32Ty, offset::vars_parent)
          };

          Value* varscope = b().CreateLoad(
              b().CreateGEP(vars_, idx, idx+2), "scope.parent");
              */

          Signature sig(ls_, ObjType);
          sig << VMTy;
          sig << "CallFrame";
          sig << ls_->Int32Ty;
          sig << ls_->Int32Ty;

          Value* call_args[] = {
            vm_,
            nfo->call_frame(),
            ConstantInt::get(ls_->Int32Ty, depth),
            ConstantInt::get(ls_->Int32Ty, index)
          };

          Value* val =
              sig.call("rbx_push_local_from", call_args, 4, "vs_uplocal", b());

          check_for_exception(val);
          stack_push(val);
        }

        return;
      }

      if(depth == 0) {
        visit_push_local(index);
        return;
      }
      /*
      else if(depth == 1) {
        Value* idx[] = {
          ConstantInt::get(ls_->Int32Ty, 0),
          ConstantInt::get(ls_->Int32Ty, offset::vars_parent)
        };

        Value* gep = b().CreateGEP(vars_, idx, idx+2, "parent_pos");

        Value* parent = b().CreateLoad(gep, "scope.parent");
        push_scope_local(parent, index);
        return;
      }
      */

      // Handle depth > 1
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ls_->Int32Ty);
      types.push_back(ls_->Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_local_depth", ft));

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(ls_->Int32Ty, depth),
        ConstantInt::get(ls_->Int32Ty, index)
      };

      Value* val = b().CreateCall(func, call_args, call_args+4, "pld");
      check_for_exception(val);
      stack_push(val);
    }

    void visit_goto(opcode ip) {
      BasicBlock* bb = block_map_[ip].block;
      assert(bb);
      b().CreateBr(bb);
      set_block(new_block("continue"));
    }

    void visit_goto_if_true(opcode ip) {
      Value* cond = stack_pop();
      Value* i = b().CreatePtrToInt(
          cond, ls_->IntPtrTy, "as_int");

      Value* anded = b().CreateAnd(i,
          ConstantInt::get(ls_->IntPtrTy, FALSE_MASK), "and");

      Value* cmp = b().CreateICmpNE(anded,
          ConstantInt::get(ls_->IntPtrTy, cFalse), "is_true");

      BasicBlock* cont = new_block("continue");
      BasicBlock* bb = block_map_[ip].block;
      assert(bb);
      b().CreateCondBr(cmp, bb, cont);

      set_block(cont);
    }

    void visit_goto_if_false(opcode ip) {
      Value* cond = stack_pop();
      Value* i = b().CreatePtrToInt(
          cond, ls_->IntPtrTy, "as_int");

      Value* anded = b().CreateAnd(i,
          ConstantInt::get(ls_->IntPtrTy, FALSE_MASK), "and");

      Value* cmp = b().CreateICmpEQ(anded,
          ConstantInt::get(ls_->IntPtrTy, cFalse), "is_true");

      BasicBlock* cont = new_block("continue");
      BasicBlock* bb = block_map_[ip].block;
      assert(bb);
      b().CreateCondBr(cmp, bb, cont);

      set_block(cont);
    }

    void visit_yield_stack(opcode count) {
      if(skip_yield_stack_) {
        skip_yield_stack_ = false;
        return;
      }

      set_has_side_effects();

      JITInlineBlock* ib = info().inline_block();

      // Hey! Look at that! We know the block we'd be yielding to
      // staticly! woo! ok, lets just emit the code for it here!
      if(ib && ib->code()) {
        context().set_inlined_block(true);

        JITMethodInfo* creator = ib->creation_scope();
        assert(creator);

        // Count the block against the policy size total
        inline_policy()->increase_size(ib->code());

        // We inline unconditionally here, since we make the decision
        // wrt the block when we are considering inlining the send that
        // has the block on it.
        Inliner inl(context(), *this, count);

        // Propagate the creator's inlined block into the inlined block.
        // This is so that if the inlined block yields, it can see the outer
        // inlined block and emit code for it.
        inl.set_inline_block(creator->inline_block());

        // Make it's inlining info available to itself
        inl.set_block_info(ib);

        inl.set_creator(creator);

        inl.inline_block(ib, get_self(creator->variables()));

        stack_remove(count);
        if(inl.check_for_exception()) {
          check_for_exception(inl.result());
        }
        stack_push(inl.result());
        return;
      }

      Value* vars = vars_;

      if(JITMethodInfo* home = info().home_info()) {
        vars = home->variables();
      }

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << "Object";
      sig << ls_->Int32Ty;
      sig << ObjArrayTy;

      Value* block_obj = b().CreateLoad(
          b().CreateConstGEP2_32(vars, 0, offset::vars_block),
          "block");

      Value* call_args[] = {
        vm_,
        call_frame_,
        block_obj,
        ConstantInt::get(ls_->Int32Ty, count),
        stack_objects(count)
      };

      flush_ip();
      Value* val = sig.call("rbx_yield_stack", call_args, 5, "ys", b());
      stack_remove(count);

      check_for_exception(val);
      stack_push(val);
    }

    void visit_yield_splat(opcode count) {
      set_has_side_effects();

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << "Object";
      sig << ls_->Int32Ty;
      sig << ObjArrayTy;

      Value* block_obj = b().CreateLoad(
          b().CreateConstGEP2_32(vars_, 0, offset::vars_block),
          "block");

      Value* call_args[] = {
        vm_,
        call_frame_,
        block_obj,
        ConstantInt::get(ls_->Int32Ty, count),
        stack_objects(count + 1)
      };

      flush_ip();
      Value* val = sig.call("rbx_yield_splat", call_args, 5, "ys", b());
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

      func->setDoesNotCapture(0, true);
      func->setDoesNotCapture(1, true);
      func->setDoesNotCapture(2, true);

      flush();

      Value* call_args[] = {
        vm_,
        call_frame_
      };

      Value* ret = b().CreateCall(func, call_args, call_args+2, "ci");
      check_for_exception(ret, false);
    }

    void visit_check_serial(opcode index, opcode serial) {
      Signature sig(ls_, "Object");
      sig << "VM";
      sig << "CallFrame";
      sig << "InlineCache";
      sig << ls_->Int32Ty;
      sig << "Object";

      Value* cache_const = b().CreateIntToPtr(
          ConstantInt::get(ls_->IntPtrTy, index),
          ptr_type("InlineCache"), "cast_to_ptr");

      Value* call_args[] = {
        vm_,
        call_frame_,
        cache_const,
        ConstantInt::get(ls_->Int32Ty, serial),
        stack_pop()
      };

      sig.setDoesNotCapture("rbx_check_serial", 2);

      stack_push(sig.call("rbx_check_serial", call_args, 5, "cs", b()));
    }

    void visit_check_serial_private(opcode index, opcode serial) {
      Signature sig(ls_, "Object");
      sig << "VM";
      sig << "CallFrame";
      sig << "InlineCache";
      sig << ls_->Int32Ty;
      sig << "Object";

      Value* cache_const = b().CreateIntToPtr(
          ConstantInt::get(ls_->IntPtrTy, index),
          ptr_type("InlineCache"), "cast_to_ptr");

      Value* call_args[] = {
        vm_,
        call_frame_,
        cache_const,
        ConstantInt::get(ls_->Int32Ty, serial),
        stack_pop()
      };

      sig.setDoesNotCapture("rbx_check_serial_private", 2);

      stack_push(sig.call("rbx_check_serial_private", call_args, 5, "cs", b()));
    }

    void visit_push_my_offset(opcode i) {
      Value* idx[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::vars_self)
      };

      Value* pos = b().CreateGEP(vars_, idx, idx+2, "self_pos");

      Value* self = b().CreateLoad(pos, "self");

      assert(i % sizeof(Object*) == 0);

      Value* cst = b().CreateBitCast(
          self,
          llvm::PointerType::getUnqual(ObjType), "obj_array");

      Value* idx2[] = {
        ConstantInt::get(ls_->Int32Ty, i / sizeof(Object*))
      };

      pos = b().CreateGEP(cst, idx2, idx2+1, "field_pos");

      stack_push(b().CreateLoad(pos, "field"));
    }

    void visit_setup_unwind(opcode where, opcode type) {
      BasicBlock* code;

      JITBasicBlock& jbb = block_map_[where];
      jbb.landing_pad = true;
      assert(jbb.block);

      if(type == cRescue) {
        // Add a prologue block that checks if we should handle this
        // exception.

        BasicBlock* orig = current_block();
        code = new_block("is_exception");
        set_block(code);

        std::vector<const Type*> types;
        types.push_back(VMTy);

        FunctionType* ft = FunctionType::get(ls_->Int1Ty, types, false);
        Function* func = cast<Function>(
            module_->getOrInsertFunction("rbx_raising_exception", ft));

        Value* call_args[] = { vm_ };
        Value* isit = b().CreateCall(func, call_args, call_args+1, "rae");

        // Chain to an existing handler.
        BasicBlock* next = 0;
        if(has_exception_handler()) {
          next = exception_handler();
        } else {
          next = bail_out_;
        }

        b().CreateCondBr(isit, jbb.block, next);

        set_block(orig);

        // Now, change jbb to point to code, so anyone branching there hits
        // the check first.
        jbb.prologue = code;
      } else {
        code = jbb.block;
      }
    }

    // Nothing, handled by pass_one
    void visit_pop_unwind() {}

    void visit_reraise() {
      if(has_exception_handler()) {
        b().CreateBr(exception_handler());
      } else {
        b().CreateBr(bail_out_);
      }

      set_block(new_block("continue"));
    }

    void visit_raise_return() {
      // Inlining a block!
      if(in_inlined_block()) {
        // We have to flush scopes before we return.
        JITMethodInfo* nfo = &info();
        while(nfo) {
          if(nfo->use_full_scope()) {
            flush_scope_to_heap(nfo->variables());
          }

          nfo = nfo->parent_info();
        }

        JITMethodInfo* creator = info().creator_info();
        assert(creator);

        creator->add_return_value(stack_pop(), b().GetInsertBlock());
        b().CreateBr(creator->return_pad());
        return;
      } else if(!info().is_block) {
        visit_ret();
        return;
      }

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_top()
      };

      sig.call("rbx_raise_return", call_args, 3, "raise_return", b());
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

      sig.call("rbx_ensure_return", call_args, 3, "ensure_return", b());
      visit_reraise();
    }

    void visit_raise_break() {
      // inlining a block!
      if(info().block_info()) {
        // TODO this won't work if we can inline methods with exception
        // handling, because we'll skip an ensures.

        BasicBlock* blk = info().block_break_loc();
        assert(blk);

        PHINode* phi = info().block_break_result();
        phi->addIncoming(stack_pop(), b().GetInsertBlock());

        b().CreateBr(blk);
        set_block(new_block("continue"));
        return;
      }

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        stack_top()
      };

      sig.call("rbx_raise_break", call_args, 3, "raise_break", b());
      visit_reraise();
    }

    void visit_push_current_exception() {
      std::vector<const Type*> types;

      types.push_back(VMTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_current_exception", ft));

      Value* call_args[] = { vm_ };

      stack_push(b().CreateCall(func, call_args, call_args+1, "ce"));
    }

    void visit_clear_exception() {
      set_has_side_effects();

      std::vector<const Type*> types;

      types.push_back(VMTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_clear_exception", ft));

      Value* call_args[] = { vm_ };

      b().CreateCall(func, call_args, call_args+1);
    }

    void visit_push_exception_state() {
      std::vector<const Type*> types;

      types.push_back(VMTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_exception_state", ft));

      Value* call_args[] = { vm_ };

      stack_push(b().CreateCall(func, call_args, call_args+1));
    }

    void visit_restore_exception_state() {
      std::vector<const Type*> types;

      types.push_back(VMTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_restore_exception_state", ft));

      Value* call_args[] = { vm_, call_frame_, stack_pop() };

      b().CreateCall(func, call_args, call_args+3);
    }

    void visit_find_const(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ls_->Int32Ty;
      sig << ObjType;

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(ls_->Int32Ty, which),
        stack_pop()
      };

      flush();

      Value* val = sig.call("rbx_find_const", call_args, 4, "constant", b());
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

      Value* val = sig.call("rbx_instance_of", call_args, 3, "constant", b());
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

      Value* val = sig.call("rbx_kind_of", call_args, 3, "constant", b());
      stack_push(val);
    }

    void visit_is_nil() {
      Value* cmp = b().CreateICmpEQ(stack_pop(),
          constant(Qnil), "is_nil");
      Value* imm_value = b().CreateSelect(cmp, constant(Qtrue),
          constant(Qfalse), "select_bool");
      stack_push(imm_value);
    }

    void visit_make_array(opcode count) {
      // Detect if we're passing an array as the arguments to an inlined
      // block, so that we can avoid the array boxing.
      if(next_op() == InstructionSequence::insn_yield_stack &&
          next_op_operand(0) == 1)
      {
        JITInlineBlock* ib = info().inline_block();

        // Hey! Look at that! We know the block we'd be yielding to
        // staticly! woo! ok, lets just emit the code for it here!
        if(ib && ib->code()) {
          skip_yield_stack_ = true; // skip the yield_stack, we're doing the work here.

          context().set_inlined_block(true);

          JITMethodInfo* creator = ib->creation_scope();
          assert(creator);

          // Count the block against the policy size total
          inline_policy()->increase_size(ib->code());

          // We inline unconditionally here, since we make the decision
          // wrt the block when we are considering inlining the send that
          // has the block on it.
          Inliner inl(context(), *this, count);
          inl.set_inline_block(creator->inline_block());
          inl.set_from_unboxed_array();

          // Make it's inlining info available to itself
          inl.set_block_info(ib);

          inl.set_creator(creator);

          inl.inline_block(ib, get_self(creator->variables()));

          stack_remove(count);
          if(inl.check_for_exception()) {
            check_for_exception(inl.result());
          }
          stack_push(inl.result());
          return;
        }

      }

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ls_->Int32Ty;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        ConstantInt::get(ls_->Int32Ty, count),
        stack_objects(count)
      };

      Value* val = sig.call("rbx_make_array", call_args, 3, "constant", b());
      stack_remove(count);
      stack_push(val);
    }

    void visit_meta_send_call(opcode name, opcode count) {
      InlineCache* cache = reinterpret_cast<InlineCache*>(name);
      if(cache->classes_seen() == 0) {
        set_has_side_effects();

        Signature sig(ls_, ObjType);

        sig << VMTy;
        sig << CallFrameTy;
        sig << ls_->Int32Ty;
        sig << ObjArrayTy;

        Value* call_args[] = {
          vm_,
          call_frame_,
          ConstantInt::get(ls_->Int32Ty, count),
          stack_objects(count + 1)
        };

        flush_ip();
        Value* val = sig.call("rbx_meta_send_call", call_args, 4, "constant", b());
        stack_remove(count+1);
        check_for_exception(val);
        stack_push(val);
      } else {
        invoke_inline_cache(cache, count);
      }
    }

    void visit_passed_arg(opcode count) {
      if(called_args_ >= 0) {
        if((int)count < called_args_) {
          stack_push(constant(Qtrue));
        } else {
          stack_push(constant(Qfalse));
        }
      } else {
        Signature sig(ls_, ObjType);

        sig << VMTy;
        sig << "Arguments";
        sig << ls_->Int32Ty;

        Value* call_args[] = {
          vm_,
          args_,
          ConstantInt::get(ls_->Int32Ty, count)
        };

        Value* val = sig.call("rbx_passed_arg", call_args, 3, "pa", b());
        stack_push(val);
      }
    }

    void visit_passed_blockarg(opcode count) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << "Arguments";
      sig << ls_->Int32Ty;

      Value* call_args[] = {
        vm_,
        args_,
        ConstantInt::get(ls_->Int32Ty, count)
      };

      Value* val = sig.call("rbx_passed_blockarg", call_args, 3, "pa", b());
      stack_push(val);
    }

    void visit_push_cpath_top() {
      push_system_object(0);
    }

    void visit_push_rubinius() {
      push_system_object(1);
    }

    void visit_push_ivar(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjType;
      sig << ObjType;

      Value* self = get_self();

      Value* call_args[] = {
        vm_,
        self,
        constant(as<Symbol>(literal(which)))
      };

      Value* val = sig.call("rbx_push_ivar", call_args, 3, "ivar", b());
      // TODO: why would rbx_push_ivar raise an exception?
      // check_for_exception(val);
      stack_push(val);
    }

    void visit_set_ivar(opcode which) {
      set_has_side_effects();

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << "CallFrame";
      sig << ObjType;
      sig << ObjType;
      sig << ObjType;

      Value* self = get_self();

      Value* call_args[] = {
        vm_,
        call_frame_,
        self,
        constant(as<Symbol>(literal(which))),
        stack_top()
      };

      Value* ret = sig.call("rbx_set_ivar", call_args, 5, "ivar", b());
      check_for_exception(ret, false);
    }

    void visit_push_my_field(opcode which) {
      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjType;
      sig << ls_->Int32Ty;

      Value* self = get_self();

      Value* call_args[] = {
        vm_,
        self,
        ConstantInt::get(ls_->Int32Ty, which)
      };

      Value* val = sig.call("rbx_push_my_field", call_args, 3, "field", b());
      check_for_exception(val);
      stack_push(val);
    }

    void visit_store_my_field(opcode which) {
      set_has_side_effects();

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjType;
      sig << ls_->Int32Ty;
      sig << ObjType;

      Value* self = get_self();

      Value* call_args[] = {
        vm_,
        self,
        ConstantInt::get(ls_->Int32Ty, which),
        stack_top()
      };

      sig.call("rbx_set_my_field", call_args, 4, "field", b());
    }

    void visit_shift_array() {
      JITStackArgs* inline_args = incoming_args();
      if(inline_args && current_hint() == cHintLazyBlockArgs) {
        if(inline_args->size() <= (size_t)block_arg_shift_) {
          stack_push(constant(Qnil));
        } else {
          stack_push(inline_args->at(block_arg_shift_));
          block_arg_shift_++;
        }
        return;
      }

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        stack_back_position(0)
      };

      Value* val = sig.call("rbx_shift_array", call_args, 2, "field", b());
      stack_push(val);
    }

    void visit_string_append() {
      set_has_side_effects();

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

      Value* str = sig.call("rbx_string_append", call_args, 3, "string", b());
      stack_push(str);
    }

    void visit_string_build(opcode count) {
      set_has_side_effects();

      Signature sig(ls_, ObjType);

      sig << VMTy;
      sig << CallFrameTy;
      sig << ls_->Int32Ty;
      sig << ObjArrayTy;

      Value* call_args[] = {
        vm_,
        call_frame_,
        ConstantInt::get(ls_->Int32Ty, count),
        stack_objects(count)
      };

      Value* str = sig.call("rbx_string_build", call_args, 4, "string", b());
      stack_remove(count);

      check_for_exception(str);
      stack_push(str);
    }
  };
}
