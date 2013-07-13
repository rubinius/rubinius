#include "instructions_util.hpp"

#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/constant_cache.hpp"
#include "builtin/mono_inline_cache.hpp"
#include "builtin/poly_inline_cache.hpp"
#include "builtin/respond_to_cache.hpp"
#include "builtin/lookuptable.hpp"
#include "version.h"

#include "llvm/jit_operations.hpp"
#include "llvm/inline.hpp"
#include "llvm/jit_context.hpp"

#include "llvm/stack_args.hpp"

#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/DerivedTypes.h>
#else
#include <llvm/DerivedTypes.h>
#endif

#include <list>

namespace rubinius {
  extern "C" Object* invoke_object_class(STATE, CallFrame* call_frame, Object** args, int arg_count);


  typedef std::list<llvm::BasicBlock*> EHandlers;

  class JITFunction : public Signature {
    llvm::Function* func_;

  public:
    JITFunction(Context* ctx)
      : Signature(ctx, ctx->VoidTy)
      , func_(0)
    {}

    void resolve(const char* name, Type* rt) {
      ret_type_ = rt;
      func_ = function(name);
    }

    Value* call(Value** start, int size, const char* inst_name, IRBuilder& b) {
      return b.CreateCall(func_, ArrayRef<Value*>(start, size), inst_name);
    }
  };

  class JITFunctions {
  public:
    JITFunction return_to_here;
    JITFunction clear_raise_value;

    JITFunctions(Context* ctx)
      : return_to_here(ctx)
      , clear_raise_value(ctx)
    {
      return_to_here
        << "State"
        << "CallFrame";

      return_to_here.resolve("rbx_return_to_here", ctx->Int1Ty);

      clear_raise_value
        << "State";

      clear_raise_value.resolve("rbx_clear_raise_value", ctx->ObjTy);
    }
  };

  class JITVisit : public VisitInstructions<JITVisit>, public JITOperations {
    JITFunctions f;
    BlockMap& block_map_;

    bool allow_private_;
    opcode call_flags_;

    // bail out destinations
    llvm::BasicBlock* bail_out_;
    llvm::BasicBlock* bail_out_fast_;

    Value* method_entry_;
    Value* args_;

    Value* ip_pos_;

    Value* vars_;
    bool use_full_scope_;

    Value* global_serial_pos;
    Value* check_global_interrupts_pos;

    int called_args_;
    int sends_done_;
    bool has_side_effects_;

    int current_ip_;
    JITInlineBlock* current_block_;
    int block_arg_shift_;
    bool skip_yield_stack_;

    JITBasicBlock* current_jbb_;

    Value* machine_code_debugging_;

  public:

    static const int cHintLazyBlockArgs = 1;

    class Unsupported {};

    JITVisit(jit::Builder* builder, JITMethodInfo& info, BlockMap& bm,
             llvm::BasicBlock* start)
      : JITOperations(builder, info, start)
      , f(builder->ctx_)
      , block_map_(bm)
      , allow_private_(false)
      , call_flags_(0)
      , method_entry_(info.profiling_entry())
      , args_(info.args())
      , vars_(info.variables())
      , use_full_scope_(false)
      , called_args_(-1)
      , sends_done_(0)
      , has_side_effects_(false)
      , current_ip_(0)
      , current_block_(NULL)
      , block_arg_shift_(0)
      , skip_yield_stack_(false)
      , current_jbb_(0)
    {}

    ~JITVisit() {
      if(current_block_) {
        delete current_block_;
      }
    }

    bool for_inlined_method() {
      return info().for_inlined_method();
    }

    void initialize() {
      BasicBlock* start = b().GetInsertBlock();

      bail_out_ = new_block("bail_out");

      Value* call_args[] = {
        state_,
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
      Value* crv = f.clear_raise_value.call(&state_, 1, "crv", b());
      if(use_full_scope_) flush_scope_to_heap(vars_);

      info().add_return_value(crv, current_block());
      b().CreateBr(info().return_pad());

      set_block(start);

      ip_pos_ = b().CreateConstGEP2_32(call_frame_, 0, offset::CallFrame::ip, "ip_pos");

      global_serial_pos = b().CreateIntToPtr(
          clong((intptr_t)llvm_state()->shared().global_serial_address()),
          llvm::PointerType::getUnqual(ctx_->Int32Ty), "cast_to_intptr");

      check_global_interrupts_pos = b().CreateIntToPtr(
          clong((intptr_t)llvm_state()->shared().check_global_interrupts_address()),
          llvm::PointerType::getUnqual(ctx_->Int8Ty), "cast_to_intptr");

      init_out_args();

      machine_code_debugging_ = constant(&info().machine_code->debugging, llvm::PointerType::getUnqual(ctx_->Int32Ty));
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
          b().CreateConstGEP2_32(call_frame_, 0, offset::CallFrame::scope, "scope_pos"),
          "scope");
    }

    Value* top_scope() {
      return b().CreateLoad(
          b().CreateConstGEP2_32(call_frame_, 0, offset::CallFrame::top_scope, "top_scope_pos"),
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

    void clear_current_block() {
      if(current_block_) {
        delete current_block_;
        current_block_ = NULL;
      }
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

      Signature brk(ctx_, ctx_->Int1Ty);
      brk << StateTy;
      brk << CallFrameTy;

      Value* call_args[] = {
        state_,
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

      Signature clear(ctx_, ObjType);
      clear << StateTy;
      Value* crv = clear.call("rbx_clear_raise_value", &state_, 1, "crv", b());

      b().CreateBr(cont);

      /////
      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 2, "possible_break");
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

      if(!llvm_state()->config().jit_check_debugging) {
        b().CreateBr(cont);
        return check_active;
      }

      Value* is_debugging = b().CreateLoad(machine_code_debugging_, "loaded_debugging_flag");

      BasicBlock* restart_in_interp = new_block("restart");

      // Check the active flag
      b().CreateCondBr(
          b().CreateICmpEQ(is_debugging, cint(1), "check_active"),
          restart_in_interp,
          cont);

      set_block(restart_in_interp);

      Value* sp = last_sp_as_int();

      flush();

      Signature sig(ctx_, "Object");

      sig << "State";
      sig << "CallFrame";
      sig << ctx_->Int32Ty;
      sig << ctx_->IntPtrTy;
      sig << "CallFrame";
      sig << "CallFrame";
      sig << ctx_->Int32Ty;

      int unwinds = emit_unwinds();

      Value* root_callframe = info().top_parent_call_frame();
      Value* creator_callframe = info().creator_call_frame();

      Value* call_args[] = {
        state_,
        call_frame_,
        cint(next_ip_),
        sp,
        root_callframe,
        creator_callframe,
        cint(unwinds),
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
      b().CreateStore(cint(current_ip_), ip_pos_);
    }

    void flush() {
      flush_ip();
      flush_stack();
    }

    /* push a global constant onto the stack
    *  0 : cpath_top (Object)
     * 1 : Rubinius module
     * 2 : Rubinius::Type module
     * 3 : Rubinius::Mirror class
     */
    void push_system_object(int which) {
      // we're calling something that returns an Object
      Signature sig(ctx_, ObjType);
      // given a system state and a 32bit int
      sig << StateTy;
      sig << ctx_->Int32Ty;

      // the actual values of which are the calling arguments
      Value* call_args[] = {
        state_,
        cint(which)
      };

      // call the function we just described using the builder
      CallInst* val = sig.call("rbx_push_system_object", call_args, 2, "so", b());
      val->setOnlyReadsMemory();
      val->setDoesNotThrow();
      stack_push(val);
    }

    // visitors.

    void visit(opcode op, opcode arg1, opcode arg2) {
      throw Unsupported();
    }

    void visit_noop() {
    }

    void visit_pop() {
      stack_remove(1);
    }

    void visit_pop_many(opcode count) {
      stack_remove(count);
    }

    void visit_push_nil() {
      stack_push(constant(cNil), type::KnownType::nil());
    }

    void visit_push_true() {
      stack_push(constant(cTrue), type::KnownType::true_());
    }

    void visit_push_undef() {
      Object** addr = llvm_state()->shared().globals.undefined.object_address();
      Value* l_addr = constant(addr, ObjArrayTy);

      stack_push(b().CreateLoad(l_addr, "undefined"));
    }

    void visit_push_false() {
      stack_push(constant(cFalse), type::KnownType::false_());
    }

    void visit_push_int(opcode arg) {
      stack_push(constant(Fixnum::from(arg)),
                 type::KnownType::fixnum(arg));
    }

    void visit_meta_push_0() {
      stack_push(constant(Fixnum::from(0)), type::KnownType::fixnum(0));
    }

    void visit_meta_push_1() {
      stack_push(constant(Fixnum::from(1)), type::KnownType::fixnum(1));
    }

    void visit_meta_push_2() {
      stack_push(constant(Fixnum::from(2)), type::KnownType::fixnum(2));
    }

    void visit_meta_push_neg_1() {
      stack_push(constant(Fixnum::from(-1)), type::KnownType::fixnum(-1));
    }

    void visit_ret() {
      if(llvm_state()->include_profiling() && method_entry_) {
        Value* test = b().CreateLoad(ctx_->profiling(), "profiling");
        BasicBlock* end_profiling = new_block("end_profiling");
        BasicBlock* cont = new_block("continue");

        b().CreateCondBr(test, end_profiling, cont);

        set_block(end_profiling);

        Signature sig(ctx_, ctx_->VoidTy);
        sig << llvm::PointerType::getUnqual(ctx_->Int8Ty);

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

        store_stack_back_position(offset, val2);
        store_stack_back_position(i, val);
      }
    }

    void visit_move_down(opcode positions) {
      Value* val = stack_top();

      for(opcode i = 0; i < positions; i++) {
        int target = i;
        int current = target + 1;

        Value* tmp = stack_back(current);
        store_stack_back_position(target, tmp);
      }

      store_stack_back_position(positions, val);
    }

    void visit_check_frozen() {
      Value* recv = stack_top();

      check_is_frozen(recv);
    }

    void check_fixnums(Value* left, Value* right, BasicBlock* if_true,
                       BasicBlock* if_false) {
      Value* mask = clong(TAG_FIXNUM_MASK);
      Value* tag  = clong(TAG_FIXNUM);

      Value* lint = cast_int(left);
      Value* rint = cast_int(right);
      Value* both =   b().CreateAnd(lint, rint, "both");
      Value* masked = b().CreateAnd(both, mask, "masked");

      Value* cmp = b().CreateICmpEQ(masked, tag, "are_fixnums");

      b().CreateCondBr(cmp, if_true, if_false);
    }

    void check_both_not_references(Value* left, Value* right, BasicBlock* if_true,
                            BasicBlock* if_false) {
      Value* mask = clong(TAG_REF_MASK);
      Value* zero = clong(TAG_REF);

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
      sig << StateTy;
      sig << CallFrameTy;
      sig << ObjType;
      sig << ctx_->IntPtrTy;
      sig << ObjArrayTy;
    }

    void setup_out_args_with_block(Symbol* name, int args) {
      b().CreateStore(stack_back(args + 1), out_args_recv_);
      b().CreateStore(constant(name, ptr_type("Symbol")), out_args_name_);
      b().CreateStore(stack_top(), out_args_block_);
      b().CreateStore(cint(args),
                    out_args_total_);
      b().CreateStore(Constant::getNullValue(ptr_type("Tuple")),
                      out_args_container_);
      if(args > 0) {
        b().CreateStore(stack_objects(args + 1), out_args_arguments_);
      }
    }

    Value* invoke_call_site(opcode& which) {
      CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&which);

      Value* call_site_ptr_const = b().CreateIntToPtr(
          clong(reinterpret_cast<uintptr_t>(call_site_ptr)),
          ptr_type(ptr_type("CallSite")), "cast_to_ptr");

      Value* call_site_const = b().CreateLoad(call_site_ptr_const, "cache_const");

      Value* execute_pos_idx[] = {
        cint(0),
        cint(offset::CallSite::executor),
      };

      Value* execute_pos = b().CreateGEP(call_site_const,
          execute_pos_idx, "execute_pos");

      Value* execute = b().CreateLoad(execute_pos, "execute");

      Value* call_args[] = {
        state_,
        call_site_const,
        call_frame_,
        out_args_
      };

      flush_ip();
      return b().CreateCall(execute, call_args, "call_site_execute");
    }

    Value* inline_call_site_execute(int args, opcode& which) {
      sends_done_++;

      CallSite* call_site = reinterpret_cast<CallSite*>(which);

      setup_out_args(call_site->name(), args);
      return invoke_call_site(which);
    }

    Value* block_send(opcode& which, int args, bool priv=false) {

      sends_done_++;

      CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&which);
      CallSite* call_site = *call_site_ptr;

      Value* call_site_ptr_const = b().CreateIntToPtr(
          clong(reinterpret_cast<uintptr_t>(call_site_ptr)),
          ptr_type(ptr_type("CallSite")), "cast_to_cache_ptr");

      Value* call_site_const = b().CreateLoad(call_site_ptr_const, "cache_const");

      Value* execute_pos_idx[] = {
        cint(0),
        cint(offset::CallSite::executor),
      };

      Value* execute_pos = b().CreateGEP(call_site_const,
          execute_pos_idx, "execute_pos");

      Value* execute = b().CreateLoad(execute_pos, "execute");

      setup_out_args_with_block(call_site->name(), args);

      Value* call_args[] = {
        state_,
        call_site_const,
        call_frame_,
        out_args_
      };

      flush_ip();
      return b().CreateCall(execute, call_args, "ic_send");
    }

    Value* splat_send(opcode& which, int args, bool priv=false) {

      sends_done_++;
      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << "CallSite";
      sig << ctx_->Int32Ty;
      sig << ctx_->Int32Ty;
      sig << ObjArrayTy;

      const char* func_name;
      if(priv) {
        func_name = "rbx_splat_send_private";
      } else {
        func_name = "rbx_splat_send";
      }

      CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&which);

      Value* call_site_ptr_const = b().CreateIntToPtr(
          clong(reinterpret_cast<uintptr_t>(call_site_ptr)),
          ptr_type(ptr_type("CallSite")), "cast_to_ptr");

      Value* call_site_const = b().CreateLoad(call_site_ptr_const, "cache_const");

      Value* call_args[] = {
        state_,
        call_frame_,
        call_site_const,
        cint(args),
        cint(call_flags_),
        stack_objects(args + 3),   // 3 == recv + block + splat
      };

      call_flags_ = 0;
      flush_ip();
      return sig.call(func_name, call_args, 6, "splat_send", b());
    }

    Value* super_send(opcode& which, int args, bool splat=false) {

      Signature sig(ctx_, ObjType);
      sig << StateTy;
      sig << CallFrameTy;
      sig << "CallSite";
      sig << ctx_->Int32Ty;
      sig << ctx_->Int32Ty;
      sig << ObjArrayTy;

      const char* func_name;
      int extra = 1;
      if(splat) {
        func_name = "rbx_super_splat_send";
        extra++;
      } else {
        func_name = "rbx_super_send";
      }

      CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&which);

      Value* call_site_ptr_const = b().CreateIntToPtr(
          clong(reinterpret_cast<uintptr_t>(call_site_ptr)),
          ptr_type(ptr_type("CallSite")), "cast_to_ptr");

      Value* call_site_const = b().CreateLoad(call_site_ptr_const, "cache_const");

      Value* call_args[] = {
        state_,
        call_frame_,
        call_site_const,
        cint(args),
        cint(call_flags_),
        stack_objects(args + extra),
      };

      call_flags_ = 0;
      flush_ip();
      return sig.call(func_name, call_args, 6, "super_send", b());
    }

    void visit_meta_to_s(opcode& name) {
      set_has_side_effects();

      Value* recv = stack_top();

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "CallFrame";
      sig << "CallSite";
      sig << "Object";

      CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&name);

      Value* call_site_ptr_const = b().CreateIntToPtr(
          clong(reinterpret_cast<uintptr_t>(call_site_ptr)),
          ptr_type(ptr_type("CallSite")), "cast_to_ptr");

      Value* call_site_const = b().CreateLoad(call_site_ptr_const, "cache_const");

      Value* args[] = {
        state_,
        call_frame_,
        call_site_const,
        recv
      };

      Value* ret = sig.call("rbx_meta_to_s", args, 4, "", b());
      stack_remove(1);
      check_for_exception(ret);
      stack_push(ret);
    }

    void visit_meta_send_op_equal(opcode& name) {
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block();

      check_both_not_references(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_call_site_execute(1, name);
      BasicBlock* send_block =
        check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* cmp = b().CreateICmpEQ(recv, arg, "imm_cmp");
      Value* imm_value = b().CreateSelect(cmp,
          constant(cTrue), constant(cFalse), "select_bool");

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 2, "equal_value");
      phi->addIncoming(called_value, send_block);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_tequal(opcode& name) {
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block();

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_call_site_execute(1, name);
      BasicBlock* send_block =
        check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* cmp = b().CreateICmpEQ(recv, arg, "imm_cmp");
      Value* imm_value = b().CreateSelect(cmp,
          constant(cTrue), constant(cFalse), "select_bool");

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 2, "equal_value");
      phi->addIncoming(called_value, send_block);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_lt(opcode& name) {
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_call_site_execute(1, name);
      BasicBlock* send_bb = check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* cmp = b().CreateICmpSLT(recv, arg, "imm_cmp");
      Value* imm_value = b().CreateSelect(cmp,
          constant(cTrue), constant(cFalse), "select_bool");

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 2, "addition");
      phi->addIncoming(called_value, send_bb);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_gt(opcode& name) {
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* cont = new_block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_call_site_execute(1, name);
      BasicBlock* send_bb = check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* cmp = b().CreateICmpSGT(recv, arg, "imm_cmp");
      Value* imm_value = b().CreateSelect(cmp,
          constant(cTrue), constant(cFalse), "select_bool");

      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 2, "compare");
      phi->addIncoming(called_value, send_bb);
      phi->addIncoming(imm_value, fast);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_plus(opcode& name) {
      set_has_side_effects();
      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast     = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* bignum   = new_block("bignum");
      BasicBlock* tagnow   = new_block("tagnow");
      BasicBlock* cont     = new_block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_call_site_execute(1, name);
      BasicBlock* send_bb = check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* recv_int = fixnum_strip(recv);
      Value* arg_int  = fixnum_strip(arg);

      Value* sum = b().CreateAdd(recv_int, arg_int, "fixnum.add");

      Value* cmp = check_if_fits_fixnum(sum);

      create_conditional_branch(tagnow, bignum, cmp);

      set_block(bignum);
      Value* big_value = promote_to_bignum(sum);
      b().CreateBr(cont);

      set_block(tagnow);
      Value* imm_value = fixnum_tag(sum);
      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 3, "addition");
      phi->addIncoming(called_value, send_bb);
      phi->addIncoming(big_value, bignum);
      phi->addIncoming(imm_value, tagnow);

      stack_remove(2);
      stack_push(phi);
    }

    void visit_meta_send_op_minus(opcode& name) {
      set_has_side_effects();

      Value* recv = stack_back(1);
      Value* arg =  stack_top();

      BasicBlock* fast     = new_block("fast");
      BasicBlock* dispatch = new_block("dispatch");
      BasicBlock* tagnow   = new_block("tagnow");
      BasicBlock* bignum   = new_block("bignum");
      BasicBlock* cont     = new_block("cont");

      check_fixnums(recv, arg, fast, dispatch);

      set_block(dispatch);

      Value* called_value = inline_call_site_execute(1, name);
      BasicBlock* send_bb = check_for_exception_then(called_value, cont);

      set_block(fast);

      Value* recv_int = fixnum_strip(recv);
      Value* arg_int  = fixnum_strip(arg);

      Value* sub = b().CreateSub(recv_int, arg_int, "fixnum.sub");

      Value* cmp = check_if_fits_fixnum(sub);

      create_conditional_branch(tagnow, bignum, cmp);

      set_block(bignum);
      Value* big_value = promote_to_bignum(sub);
      b().CreateBr(cont);

      set_block(tagnow);
      Value* imm_value = fixnum_tag(sub);
      b().CreateBr(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 3, "subtraction");
      phi->addIncoming(called_value, send_bb);
      phi->addIncoming(big_value, bignum);
      phi->addIncoming(imm_value, tagnow);

      stack_remove(2);
      stack_push(phi);
    }

    Object* literal(opcode which) {
      return info().method()->literals()->at(which);
    }

    Value* get_literal(opcode which) {
      Value* gep = b().CreateConstGEP2_32(call_frame_, 0,
          offset::CallFrame::compiled_code, "code_pos");
      Value* code =  b().CreateLoad(gep, "code");

      gep = b().CreateConstGEP2_32(code, 0, offset::CompiledCode::literals, "literals_pos");
      Value* lits = b().CreateLoad(gep, "literals");

      Value* idx2[] = {
        cint(0),
        cint(offset::Tuple::field),
        cint(which)
      };

      gep = b().CreateGEP(lits, idx2, "literal_pos");
      return b().CreateLoad(gep, "literal");
    }

    void visit_push_literal(opcode which) {
      Object* lit = literal(which);
      if(Symbol* sym = try_as<Symbol>(lit)) {
        stack_push(constant(sym), type::KnownType::symbol(sym->index()));
      } else if(Fixnum* fix = try_as<Fixnum>(lit)) {
        stack_push(constant(fix), type::KnownType::fixnum(fix->to_native()));
      } else if(lit == cTrue) {
        stack_push(constant(cTrue), type::KnownType::true_());
      } else if(lit == cFalse) {
        stack_push(constant(cFalse), type::KnownType::false_());
      } else if(lit == cNil) {
        stack_push(constant(cNil), type::KnownType::nil());
      } else if(lit->reference_p()) {
        Class* klass = lit->klass();
        type::KnownType kt = type::KnownType::unknown();
        if(kind_of<SingletonClass>(klass)) {
          kt = type::KnownType::singleton_instance(klass->class_id());
        } else {
          kt = type::KnownType::instance(klass->class_id());
        }
        stack_push(get_literal(which), kt);
      } else {
        stack_push(get_literal(which));
      }
    }

    void visit_string_dup() {
      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_string_dup", ft));
      func->setDoesNotCapture(1);
      func->setDoesNotCapture(2);
      func->setDoesNotCapture(3);
      func->setDoesNotAlias(0);

      Value* call_args[] = {
        state_,
        call_frame_,
        stack_pop()
      };

      Value* dup = b().CreateCall(func, call_args, "string_dup");
      check_for_exception(dup);
      stack_push(dup, type::KnownType::instance(llvm_state()->string_class_id()));
    }

    void push_scope_local(Value* scope, opcode which) {
      Value* pos = b().CreateConstGEP2_32(scope, 0, offset::VariableScope::locals,
                                     "locals_pos");

      Value* table = b().CreateLoad(pos, "locals");

      Value* val_pos = b().CreateConstGEP1_32(table, which, "local_pos");

      stack_push(b().CreateLoad(val_pos, "local"));
    }

    Value* local_location(Value* vars, opcode which) {
      Value* idx2[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        cint(which)
      };

      return b().CreateGEP(vars, idx2, "local_pos");
    }

    void visit_push_stack_local(opcode which) {
      Value* pos = stack_slot_position(machine_code()->stack_size - which - 1);
      stack_push(b().CreateLoad(pos, "stack_local"));
    }

    void visit_set_stack_local(opcode which) {
      Value* pos = stack_slot_position(machine_code()->stack_size - which - 1);
      b().CreateStore(stack_top(), pos);
    }

    void visit_push_local(opcode which) {
      Value* idx2[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        cint(which)
      };

      Value* pos = b().CreateGEP(vars_, idx2, "local_pos");

      if(LocalInfo* bli = current_jbb_->get_local(which)) {
        type::KnownType kt = bli->known_type();
        kt.set_local_source(which);
        stack_push(b().CreateLoad(pos, "local"), kt);
        return;
      }

      LocalInfo* li = info().get_local(which);
      type::KnownType kt = li->known_type();
      kt.set_local_source(which);
      stack_push(b().CreateLoad(pos, "local"), kt);
    }

    void set_scope_local(Value* scope, opcode which) {
      Value* pos = b().CreateConstGEP2_32(scope, 0, offset::VariableScope::locals,
                                     "locals_pos");

      Value* table = b().CreateLoad(pos, "locals");

      Value* val_pos = b().CreateConstGEP1_32(table, which, "local_pos");

      Value* val = stack_top();

      b().CreateStore(val, val_pos);

      write_barrier(scope, val);
    }

    void visit_set_local(opcode which) {
      Value* idx2[] = {
        cint(0),
        cint(offset::StackVariables::locals),
        cint(which)
      };

      Value* pos = b().CreateGEP(vars_, idx2, "local_pos");

      Value* val;

      JITStackArgs* inline_args = incoming_args();
      if(inline_args && current_hint() == cHintLazyBlockArgs) {
        std::vector<Type*> types;
        types.push_back(ctx_->ptr_type("State"));
        types.push_back(ctx_->Int32Ty);

        FunctionType* ft = FunctionType::get(ctx_->ptr_type("Object"), types, true);
        Function* func = cast<Function>(
            ctx_->module()->getOrInsertFunction("rbx_create_array", ft));

        std::vector<Value*> outgoing_args;
        outgoing_args.push_back(state_);

        int ary_size;
        if(block_arg_shift_ >= (int)inline_args->size()) {
          ary_size = 0;
        } else {
          ary_size = (int)inline_args->size() - block_arg_shift_;
        }

        outgoing_args.push_back(cint(ary_size));

        if(ary_size > 0) {
          for(size_t i = block_arg_shift_; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }
        }

        val = b().CreateCall(func, outgoing_args, "ary");
      } else {
        val = stack_top();
        LocalInfo* li = info().get_local(which);
        type::KnownType kt = type::KnownType::extract(ctx_, val);
        li->set_known_type(kt);
      }

      b().CreateStore(val, pos);
    }

    Instruction* get_self(Value* vars = 0) {
      if(!vars) vars = vars_;

      assert(vars);

      return b().CreateLoad(
          b().CreateConstGEP2_32(vars, 0, offset::StackVariables::self),
          "self");
    }

    Value* get_block() {
      return b().CreateLoad(
          b().CreateConstGEP2_32(vars_, 0, offset::StackVariables::block, "block_pos"));
    }

    void visit_push_self() {
      Instruction* val = get_self();

      info().self_type.associate(ctx_, val);

      stack_push(val);
    }

    void visit_allow_private() {
      allow_private_ = true;
    }

    void visit_set_call_flags(opcode flag) {
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

      Signature sig(ctx_, ctx_->VoidTy);

      sig << "State";
      sig << ctx_->Int32Ty;
      sig << ctx_->Int32Ty;
      sig << ctx_->Int32Ty;
      sig << ctx_->Int32Ty;

      for(int i = count - 1; i >= 0; --i) {
        Value* call_args[] = {
          state_,
          cint(i),
          cint(jbb->start_ip),
          cint(jbb->sp),
          cint(jbb->exception_type)
        };
        sig.call("rbx_setup_unwind", call_args, 5, "", b());
        jbb = jbb->exception_handler;
      }

      return count;
    }

    void emit_uncommon(bool force_deoptimization = false) {
      emit_delayed_create_block();

      if(current_block_) {
        Signature sig(ctx_, ObjType);
        sig << StateTy;
        sig << CallFrameTy;
        sig << ctx_->Int32Ty;

        Value* call_args[] = {
          state_,
          call_frame_,
          cint(current_block_->which())
        };

        Value* blk = sig.call("rbx_create_block", call_args, 3,
            "delayed_create_block", b());

        b().CreateStore(
            blk,
            current_block_->stack_ptr());
      }

      Value* sp = last_sp_as_int();

      flush();

      Signature sig(ctx_, "Object");

      sig << "State";
      sig << "CallFrame";
      sig << ctx_->Int32Ty;
      sig << ctx_->IntPtrTy;
      sig << "CallFrame";
      sig << "CallFrame";
      sig << ctx_->VoidPtrTy;
      sig << ctx_->Int32Ty;
      sig << ctx_->Int8Ty;

      int unwinds = emit_unwinds();

      Value* root_callframe    = info().top_parent_call_frame();
      Value* creator_callframe = info().creator_call_frame();

      Value* call_args[] = {
        state_,
        call_frame_,
        cint(current_ip_),
        sp,
        root_callframe,
        creator_callframe,
        constant(ctx_->runtime_data_holder(), ctx_->VoidPtrTy),
        cint(unwinds),
        llvm::ConstantInt::get(ctx_->Int8Ty, force_deoptimization)
      };

      Value* call = sig.call("rbx_continue_uncommon", call_args, 9, "", b());

      info().add_return_value(call, current_block());
      b().CreateBr(info().return_pad());
    }

    void emit_respond_to(RespondToCache* cache, opcode& which, opcode args) {
      BasicBlock* cont = new_block("check_class_respond_to");
      BasicBlock* success = new_block("success");
      BasicBlock* failure = new_block("use_call");

      Value* responds = NULL;

      if(args >= 1 && args <= 2) {
        Value* msg  = stack_back(0);
        Value* recv = stack_back(1);
        Value* priv = constant(cFalse, ObjType);
        if(args == 2) {
          priv = stack_back(1);
          recv = stack_back(2);
        }

        Class* klass = cache->receiver_class();
        check_class(recv, klass,
                          cache->receiver_class_id(),
                          cache->receiver_serial_id(),
                          failure,
                          failure);

        Value* msg_match  = b().CreateICmp(ICmpInst::ICMP_EQ, msg, constant(cache->message()), "check_message");
        Value* priv_match = b().CreateICmp(ICmpInst::ICMP_EQ, priv, constant(cache->visibility()), "check_visibility");
        Value* both_match = create_and(msg_match, priv_match, "both_match");

        create_conditional_branch(success, failure, both_match);
        set_block(success);

        responds = constant(cache->responds());
        create_branch(cont);
      } else {
        create_branch(failure);
      }

      set_block(failure);

      Value* ret = inline_call_site_execute(args, which);
      check_for_exception(ret);
      failure = current_block();

      create_branch(cont);
      set_block(cont);

      stack_remove(args + 1);

      if(responds) {
        PHINode* phi = b().CreatePHI(ObjType, 2, "object_class");
        phi->addIncoming(responds, success);
        phi->addIncoming(ret, failure);
        stack_push(phi);
      } else {
        stack_push(ret);
      }
    }

    void visit_invoke_primitive(opcode which, opcode args) {
      InvokePrimitive invoker = reinterpret_cast<InvokePrimitive>(which);

      BasicBlock* fin = 0;
      Value* inline_klass = 0;
      BasicBlock* inline_body = 0;

      if(invoker == invoke_object_class && args == 1) {
        Value* obj = stack_back(0);

        type::KnownType kt = type::KnownType::extract(ctx_, obj);
        if(kt.instance_p() && !kt.singleton_instance_p()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->inline_log("inlining") << "direct class of reference\n";
          }

          stack_remove(1);

          inline_klass = b().CreateBitCast(reference_class(obj), ObjType);
          stack_push(inline_klass, type::KnownType::class_object(kt.class_id()));
          return;
        }

        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inlining") << "custom object_class invoker\n";
        }

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

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "CallFrame";
      sig << ObjArrayTy;
      sig << ctx_->Int32Ty;

      Value* arg_ary = stack_objects(args);

      Value* call_args[] = { state_, call_frame_, arg_ary, cint(args) };

      Value* ptr = b().CreateIntToPtr(
          clong(reinterpret_cast<uintptr_t>(invoker)),
          llvm::PointerType::getUnqual(sig.type()));

      Value* call = b().CreateCall(ptr, call_args, "invoked_prim");
      stack_remove(args);

      check_for_exception(call);

      if(fin) {
        BasicBlock* cur = current_block();

        b().CreateBr(fin);

        set_block(fin);

        PHINode* phi = b().CreatePHI(ObjType, 2, "object_class");
        phi->addIncoming(inline_klass, inline_body);
        phi->addIncoming(call, cur);

        stack_push(phi);
      } else {
        stack_push(call);
      }
    }

    void invoke_call_site(opcode& call_site, opcode args) {
      set_has_side_effects();

      Value* ret = inline_call_site_execute(args, call_site);
      stack_remove(args + 1);
      check_for_exception(ret);
      stack_push(ret);

      allow_private_ = false;
    }

    void visit_send_stack(opcode& which, opcode args) {
      CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&which);
      CallSite*  call_site = *call_site_ptr;

      if(RespondToCache* respond_to = try_as<RespondToCache>(call_site)) {
        emit_respond_to(respond_to, which, args);
        return;
      }

      if(!call_site->regular_call()) {
        invoke_call_site(which, args);
        return;
      }

      BasicBlock* class_failure = new_block("class_fallback");
      BasicBlock* serial_failure = new_block("serial_fallback");
      BasicBlock* cont = new_block("continue");

      Inliner inl(ctx_, *this, call_site_ptr, args, class_failure, serial_failure);

      MonoInlineCache* mono = try_as<MonoInlineCache>(call_site);
      PolyInlineCache* poly = try_as<PolyInlineCache>(call_site);

      bool res = false;
      if(mono) {
        res = inl.consider_mono();
      } else if(poly) {
        res = inl.consider_poly();
      }

      // If we have tried to reoptimize here a few times and failed, we use
      // a regular send as the fallback so we don't try to keep reoptimizing in
      // the future.
      if(poly && poly->seen_classes_overflow() > llvm_state()->shared().config.jit_deoptimize_overflow_threshold) {
        inl.use_send_for_failure();
      }

      if(!res) {
        invoke_call_site(which, args);
        return;
      }

      BasicBlock* cur = b().GetInsertBlock();

      if(!inl.fail_to_send() && !in_inlined_block()) {

        set_block(class_failure);
        emit_uncommon();

        set_block(serial_failure);
        emit_uncommon(true);

        set_block(cur);
        stack_remove(args+1);
        if(inl.check_for_exception()) {
          check_for_exception(inl.result());
        }
        stack_push(inl.result());

        if(mono) {
          type::KnownType kt = inl.guarded_type();

          if(kt.local_source_p() && kt.known_p()) {
            current_jbb_->add_local(kt.local_id(), kt);
          }
        }

        b().CreateBr(cont);

        set_block(cont);

      } else {
        set_block(serial_failure);
        emit_uncommon(true);

        set_block(cur);

        // Emit both the inlined code and a send for it
        if(inl.check_for_exception()) {
          check_for_exception(inl.result());
        }
        BasicBlock* inline_block = b().GetInsertBlock();
        b().CreateBr(cont);

        set_block(class_failure);
        Value* send_res = inline_call_site_execute(args, which);

        BasicBlock* send_block =
          check_for_exception_then(send_res, cont);

        set_block(cont);
        PHINode* phi = b().CreatePHI(ObjType, 2, "send_result");
        phi->addIncoming(inl.result(), inline_block);
        phi->addIncoming(send_res, send_block);

        stack_remove(args + 1);

        stack_push(phi);
      }

      allow_private_ = false;
    }

    void visit_call_custom(opcode& which, opcode args) {
      if(llvm_state()->config().jit_inline_debug) {
        ctx_->log() << "generate: call_custom\n";
      }

      set_has_side_effects();

      Value* ret = inline_call_site_execute(args, which);
      stack_remove(args + 1);
      check_for_exception(ret);
      stack_push(ret);

      allow_private_ = false;
    }

    void visit_send_method(opcode& which) {
      visit_send_stack(which, 0);
    }

    bool in_inlined_block() {
      return for_inlined_method() && info().is_block;
    }

    void emit_delayed_create_block() {
      std::list<JITMethodInfo*> in_scope;
      JITMethodInfo* nfo = &info();

      while(nfo) {
        in_scope.push_front(nfo);
        nfo = nfo->parent_info();
      }

      for(std::list<JITMethodInfo*>::iterator i = in_scope.begin();
          i != in_scope.end();
          ++i) {
        nfo = *i;

        JITInlineBlock* ib = nfo->inline_block();
        if(ib && ib->machine_code()) {
          JITMethodInfo* creator = ib->creation_scope();
          assert(creator);

          Signature sig(ctx_, ObjType);
          sig << StateTy;
          sig << CallFrameTy;
          sig << ctx_->Int32Ty;

          Value* call_args[] = {
            state_,
            creator->call_frame(),
            cint(ib->which())
          };

          Value* blk = sig.call("rbx_create_block", call_args, 3,
              "delayed_create_block", b());

          b().CreateStore(
              blk,
              b().CreateConstGEP2_32(nfo->variables(), 0,
                offset::StackVariables::block),
              false);
        }
      }
    }

    void emit_create_block(opcode which) {
      // if we're inside an inlined method that has a block
      // visible, that means that we've note yet emitted the code to
      // actually create the block for this inlined block.
      //
      // But, because we're about to create a block here, it might
      // want to yield (ie, inlining Enumerable#find on an Array, but
      // not inlining the call to each inside find).
      //
      // So at this point, we have to create the block object
      // for this parent block.

      emit_delayed_create_block();

      std::vector<Type*> types;
      types.push_back(StateTy);

      // we use stack_set_top here because we always have a placeholder
      // on the stack that we're going to just replace.

      if(in_inlined_block()) {
        types.push_back(ObjType);
        types.push_back(ctx_->Int32Ty);

        FunctionType* ft = FunctionType::get(ObjType, types, true);
        Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_create_block_multi", ft));

        std::vector<Value*> call_args;
        call_args.push_back(state_);
        call_args.push_back(get_literal(which));

        std::vector<JITMethodInfo*> mis;

        JITMethodInfo* nfo = &info();
        while(nfo) {
          mis.push_back(nfo);
          nfo = nfo->creator_info();
        }

        call_args.push_back(cint(mis.size()));

        for(std::vector<JITMethodInfo*>::reverse_iterator i = mis.rbegin();
            i != mis.rend();
            ++i) {
          call_args.push_back((*i)->call_frame());
        }

        stack_set_top(b().CreateCall(func, call_args, "create_block"));
        return;
      };

      types.push_back(CallFrameTy);
      types.push_back(ctx_->Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_create_block", ft));

      Value* call_args[] = {
        state_,
        call_frame_,
        cint(which)
      };

      stack_set_top(b().CreateCall(func, call_args, "create_block"));
    }

    void visit_create_block(opcode which) {
      stack_push(constant(cNil));
      BasicBlock* block_emit = new_block("block_emit");
      b().CreateBr(block_emit);
      set_block(block_emit);

      emit_create_block(which);

      BasicBlock* block_continue = new_block("block_continue");
      b().CreateBr(block_continue);
      set_block(block_continue);

      CompiledCode* block_code = as<CompiledCode>(literal(which));
      MachineCode* code = block_code->machine_code();

      current_block_ = new JITInlineBlock(ctx_, block_code, code, &info(), which, stack_ptr());
      current_block_->set_block_emit_loc(block_emit);
    }

    void visit_send_stack_with_block(opcode& which, opcode args) {
      set_has_side_effects();

      CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&which);
      CallSite*  call_site = *call_site_ptr;
      CompiledCode* block_code = 0;

      if(call_site->regular_call() &&
          llvm_state()->config().jit_inline_blocks &&
         !ctx_->inlined_block()) {
        if(current_block_) {
          block_code = current_block_->method();

          // Run the policy on the block code here, if we're not going to
          // inline it, don't inline this either.
          InlineOptions opts;
          if(LANGUAGE_18_ENABLED) {
            opts.inlining_block();
          } else {
            opts.inlining_block_19();
          }

          InlineDecision decision = inline_policy()->inline_p(
                                      block_code->machine_code(), opts);
          if(decision == cTooComplex) {
            if(llvm_state()->config().jit_inline_debug) {
              ctx_->inline_log("NOT inlining")
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
        BasicBlock* class_failure = new_block("class_fallback");
        BasicBlock* serial_failure = new_block("serial_fallback");

        BasicBlock* cont = new_block("continue");
        BasicBlock* cleanup = new_block("send_done");
        PHINode* send_result = b().CreatePHI(ObjType, 1, "send_result");

        Inliner inl(ctx_, *this, call_site_ptr, args, class_failure, serial_failure);

        current_block_->set_block_break_result(send_result);
        current_block_->set_block_break_loc(cleanup);

        inl.set_inline_block(current_block_);

        int stack_cleanup = args + 2;

        // So that the inliner can find recv and args properly.
        inl.set_block_on_stack();

        if(inl.consider_mono()) {
          if(!inl.fail_to_send() && !in_inlined_block()) {
            send_result->addIncoming(inl.result(), b().GetInsertBlock());

            b().CreateBr(cleanup);

            set_block(class_failure);
            emit_uncommon();

            set_block(serial_failure);
            emit_uncommon(true);

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

            current_block_->eraseBlockEmit();
          } else {
            // Emit both the inlined code and a send for it

            if(inl.check_for_exception()) {
              check_for_exception(inl.result());
            }
            send_result->addIncoming(inl.result(), b().GetInsertBlock());

            b().CreateBr(cleanup);

            set_block(class_failure);

            Value* send_res = block_send(which, args, allow_private_);

            BasicBlock* send_block =
              check_for_exception_then(send_res, cleanup);

            set_block(serial_failure);
            emit_uncommon(true);

            set_block(cleanup);
            send_result->removeFromParent();
            cleanup->getInstList().push_back(send_result);

            send_result->addIncoming(send_res, send_block);

            stack_remove(stack_cleanup);
            check_for_exception(send_result);

            stack_push(send_result);
          }

          allow_private_ = false;

          // Clear the current block
          clear_current_block();
          return;
        }

        // Don't need it.
        send_result->eraseFromParent();
      }

use_send:

      Value* ret = block_send(which, args, allow_private_);
      stack_remove(args + 2);
      check_for_return(ret);
      allow_private_ = false;

      clear_current_block();
    }

    void visit_send_stack_with_splat(opcode& which, opcode args) {
      set_has_side_effects();

      Value* ret = splat_send(which, args, allow_private_);
      stack_remove(args + 3);
      check_for_exception(ret);
      stack_push(ret);
      allow_private_ = false;

      clear_current_block();
    }

    void visit_cast_array() {
      Value* top = stack_pop();

      BasicBlock* cont = new_block("cont");
      BasicBlock* done = new_block("done");
      BasicBlock* ref  = new_block("ref");
      BasicBlock* call = new_block("call");

      Value* is_ref = check_is_reference(top);
      create_conditional_branch(ref, call, is_ref);

      set_block(ref);
      Value* is_ary = check_type_bits(top, rubinius::Array::type, "is_ary");

      create_conditional_branch(done, call, is_ary);

      set_block(done);
      create_branch(cont);

      set_block(call);
      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_cast_array", ft));

      Value* call_args[] = {
        state_,
        call_frame_,
        top
      };

      Value* val = b().CreateCall(func, call_args, "cast_array");
      check_for_exception(val);

      call = b().GetInsertBlock();
      create_branch(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 2, "result");
      phi->addIncoming(top, done);
      phi->addIncoming(val, call);

      stack_push(phi);
    }

    void visit_cast_multi_value() {
      Value* top = stack_pop();

      BasicBlock* cont = new_block("cont");
      BasicBlock* done = new_block("done");
      BasicBlock* ref  = new_block("ref");
      BasicBlock* call = new_block("call");

      Value* is_ref = check_is_reference(top);
      create_conditional_branch(ref, call, is_ref);

      set_block(ref);

      Value* is_ary = check_type_bits(top, rubinius::Array::type, "is_ary");

      create_conditional_branch(done, call, is_ary);

      set_block(done);
      create_branch(cont);

      set_block(call);

      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_cast_multi_value", ft));

      Value* call_args[] = {
        state_,
        call_frame_,
        top
      };

      Value* val = b().CreateCall(func, call_args, "cast_multi_value");
      check_for_exception(val);
      call = b().GetInsertBlock();
      create_branch(cont);

      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 2, "result");
      phi->addIncoming(top, done);
      phi->addIncoming(val, call);

      stack_push(phi);

    }

    void visit_push_block() {
      stack_push(get_block());
    }

    void visit_push_has_block() {
      // We're in an inlined method, we know if there is a block statically.
      if(info().for_inlined_method()) {
        // Check for an inlined block being present, if so, this is a
        // constant!
        //
        JITInlineBlock* ib = info().inline_block();

        if(ib && ib->machine_code()) {
          stack_push(constant(cTrue));
        } else {
          stack_push(constant(cFalse));
        }

        return;
      }

      Value* cmp = b().CreateICmpNE(get_block(), constant(cNil), "is_nil");
      Value* imm_value = b().CreateSelect(cmp, constant(cTrue),
          constant(cFalse), "select_bool");
      stack_push(imm_value);
    }

    void visit_push_proc() {
      set_has_side_effects();

      Signature sig(ctx_, ObjType);
      sig << "State";
      sig << "CallFrame";

      Value* args[] = {
        state_,
        call_frame_
      };

      Value* prc = sig.call("rbx_make_proc", args, 2, "proc", b());
      check_for_exception(prc);
      stack_push(prc);
    }

    void visit_send_super_stack_with_block(opcode& which, opcode args) {
      set_has_side_effects();

      CallSite* call_site = reinterpret_cast<CallSite*>(which);

      b().CreateStore(get_self(), out_args_recv_);
      b().CreateStore(constant(call_site->name(), ptr_type("Symbol")), out_args_name_);
      b().CreateStore(stack_top(), out_args_block_);
      b().CreateStore(cint(args), out_args_total_);
      b().CreateStore(Constant::getNullValue(ptr_type("Tuple")),
                      out_args_container_);

      if(args > 0) {
        b().CreateStore(stack_objects(args + 1), out_args_arguments_);
      }

      Value* ret = invoke_call_site(which);
      stack_remove(args + 1);
      check_for_return(ret);

      clear_current_block();
    }

    void visit_send_super_stack_with_splat(opcode& which, opcode args) {
      set_has_side_effects();

      Value* ret = super_send(which, args, true);
      stack_remove(args + 2);
      check_for_exception(ret);
      stack_push(ret);

      clear_current_block();
    }

    void visit_zsuper(opcode& which) {
      set_has_side_effects();

      Signature sig(ctx_, ObjType);
      sig << StateTy;
      sig << CallFrameTy;
      sig << "CallSite";
      sig << ObjType;

      CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&which);

      Value* call_site_ptr_const = b().CreateIntToPtr(
          clong(reinterpret_cast<uintptr_t>(call_site_ptr)),
          ptr_type(ptr_type("CallSite")), "cast_to_ptr");

      Value* call_site_const = b().CreateLoad(call_site_ptr_const, "cache_const");

      Value* call_args[] = {
        state_,
        call_frame_,
        call_site_const,
        stack_top()
      };

      flush_ip();
      Value* ret = sig.call("rbx_zsuper_send", call_args, 4, "super_send", b());
      stack_remove(1);

      check_for_exception(ret);
      stack_push(ret);

      clear_current_block();
    }

    void visit_add_scope() {
      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_add_scope", ft));

      Value* call_args[] = {
        state_,
        call_frame_,
        stack_pop()
      };

      b().CreateCall(func, call_args);
    }

    Object* current_literal(opcode which) {
      return info().method()->literals()->at(which);
    }

    void visit_push_const_fast(opcode& name) {
      set_has_side_effects();

      BasicBlock* cont = 0;

      Value* cached_value = 0;
      BasicBlock* cached_block = 0;

      ConstantCache** constant_cache_ptr = reinterpret_cast<ConstantCache**>(&name);
      ConstantCache* constant_cache = *constant_cache_ptr;

      Value* global_serial = b().CreateLoad(global_serial_pos, "global_serial");


      Value* cache_ptr = b().CreateIntToPtr(
        ConstantInt::get(context()->IntPtrTy, (reinterpret_cast<uintptr_t>(constant_cache_ptr))),
        ptr_type(ptr_type("ConstantCache")), "cast_to_ptr");

      Value* cache = b().CreateLoad(cache_ptr, "constant_cache");

      Value* constant_scope_pos_idx[] = {
        context()->cint(0),
        context()->cint(offset::ConstantCache::scope),
      };

      Value* constant_scope_pos = b().CreateGEP(cache,
          constant_scope_pos_idx, "constant_cache_pos");

      Value* constant_scope = b().CreateLoad(constant_scope_pos, "constant_scope");

      Value* frame_scope = b().CreateLoad(
          b().CreateConstGEP2_32(call_frame_, 0,
                                 offset::CallFrame::constant_scope, "scope_pos"), "frame_scope");

      Value* scope_cmp = b().CreateICmpEQ(constant_scope, frame_scope, "same_scope");

      BasicBlock* check_serial = new_block("check_serial");
      BasicBlock* use_cache = new_block("use_cache");
      BasicBlock* use_call  = new_block("use_call");
      cont =      new_block("continue");

      b().CreateCondBr(scope_cmp, check_serial, use_call);

      set_block(check_serial);

      Value* serial_pos_idx[] = {
        context()->cint(0),
        context()->cint(offset::ConstantCache::serial),
      };

      Value* serial_pos = b().CreateGEP(cache,
          serial_pos_idx, "serial_pos");

      Value* current_serial = b().CreateLoad(serial_pos, "serial");

      Value* cmp = b().CreateICmpEQ(global_serial, current_serial, "use_cache");


      b().CreateCondBr(cmp, use_cache, use_call);

      set_block(use_cache);

      Value* value_pos_idx[] = {
        context()->cint(0),
        context()->cint(offset::ConstantCache::value),
      };

      Value* value_pos = b().CreateGEP(cache,
          value_pos_idx, "value_pos");

      cached_value = b().CreateLoad(value_pos, "cached_value");
      cached_block = b().GetInsertBlock();

      b().CreateBr(cont);

      set_block(use_call);

      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ptr_type(ptr_type("ConstantCache")));

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_const_fast", ft));

      func->setOnlyReadsMemory();
      func->setDoesNotThrow();

      flush();

      Value* call_args[] = {
        state_,
        call_frame_,
        constant(constant_cache_ptr, ptr_type(ptr_type("ConstantCache")))
      };

      CallInst* ret = b().CreateCall(func, call_args,
                                       "push_const_fast");

      ret->setOnlyReadsMemory();
      ret->setDoesNotThrow();

      check_for_exception(ret);

      BasicBlock* ret_block = b().GetInsertBlock();
      b().CreateBr(cont);
      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 2, "constant");
      phi->addIncoming(cached_value, cached_block);
      phi->addIncoming(ret, ret_block);

      stack_push(phi, type::KnownType::constant_cache(constant_cache));
    }

    void visit_push_const(opcode name) {
      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_const", ft));

      flush();

      Value* call_args[] = {
        state_,
        call_frame_,
        constant(as<Symbol>(literal(name)))
      };

      CallInst* ret = b().CreateCall(func, call_args, "push_const");
      ret->setOnlyReadsMemory();
      ret->setDoesNotThrow();

      check_for_exception(ret);
      stack_push(ret);
    }

    void visit_set_const(opcode name) {
      set_has_side_effects();

      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_const", ft));

      Value* call_args[] = {
        state_,
        call_frame_,
        constant(as<Symbol>(literal(name))),
        stack_top()
      };

      b().CreateCall(func, call_args);
    }

    void visit_set_const_at(opcode name) {
      set_has_side_effects();

      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(ObjType);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_const_at", ft));

      Value* val = stack_pop();
      Value* call_args[] = {
        state_,
        constant(as<Symbol>(literal(name))),
        stack_top(),
        val
      };

      b().CreateCall(func, call_args);

      stack_push(val);
    }

    void visit_set_literal(opcode which) {
      set_has_side_effects();

      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ctx_->Int32Ty);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_literal", ft));

      Value* call_args[] = {
        state_,
        call_frame_,
        cint(which),
        stack_top()
      };

      b().CreateCall(func, call_args);
    }

    void visit_push_variables() {
      set_has_side_effects();

      Signature sig(ctx_, ObjType);
      sig << "State";
      sig << "CallFrame";

      Value* args[] = {
        state_,
        call_frame_
      };

      stack_push(sig.call("rbx_promote_variables", args, 2, "promo_vars", b()));
    }

    void visit_push_scope() {
      Value* scope = b().CreateLoad(
          b().CreateConstGEP2_32(call_frame_, 0,
                                 offset::CallFrame::constant_scope, "scope_pos"),
          "constant_scope");

      stack_push(scope);
    }

    void visit_cast_for_single_block_arg() {
      JITStackArgs* inline_args = incoming_args();
      if(inline_args) {
        switch(inline_args->size()) {
        case 0:
          stack_push(constant(cNil));
          break;
        case 1:
          stack_push(inline_args->at(0));
          break;
        default: {
          std::vector<Type*> types;
          types.push_back(ctx_->ptr_type("State"));
          types.push_back(ctx_->Int32Ty);

          FunctionType* ft = FunctionType::get(ctx_->ptr_type("Object"), types, true);
          Function* func = cast<Function>(
              ctx_->module()->getOrInsertFunction("rbx_create_array", ft));

          std::vector<Value*> outgoing_args;
          outgoing_args.push_back(state_);
          outgoing_args.push_back(cint(inline_args->size()));

          for(size_t i = 0; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }

          Value* ary =
            b().CreateCall(func, outgoing_args, "ary");
          stack_push(ary);
         }
        }
      } else {
        std::vector<Type*> types;

        types.push_back(StateTy);
        types.push_back(ptr_type("Arguments"));

        FunctionType* ft = FunctionType::get(ObjType, types, false);
        Function* func = cast<Function>(
            module_->getOrInsertFunction("rbx_cast_for_single_block_arg", ft));

        Value* call_args[] = {
          state_,
          args_
        };

        stack_push(b().CreateCall(func, call_args, "cfsba"));
      }
    }

    void visit_cast_for_multi_block_arg() {
      JITStackArgs* inline_args = incoming_args();
      if(inline_args) {
        if(inline_args->size() == 1) {
          std::vector<Type*> types;
          types.push_back(ctx_->ptr_type("State"));
          types.push_back(CallFrameTy);
          types.push_back(ctx_->Int32Ty);

          FunctionType* ft = FunctionType::get(ctx_->ptr_type("Object"), types, true);
          Function* func = cast<Function>(
              ctx_->module()->getOrInsertFunction("rbx_cast_for_multi_block_arg_varargs", ft));

          std::vector<Value*> outgoing_args;
          outgoing_args.push_back(state_);
          outgoing_args.push_back(call_frame_);
          outgoing_args.push_back(cint(inline_args->size()));

          for(size_t i = 0; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }

          Value* ary =
            b().CreateCall(func, outgoing_args, "ary");
          check_for_exception(ary);
          stack_push(ary);
        } else {
          stack_push(constant(cUndef)); // holder
          set_hint(cHintLazyBlockArgs);
        }
      } else {
        Signature sig(ctx_, ObjType);
        sig << StateTy;
        sig << CallFrameTy;
        sig << ptr_type("Arguments");

        Value* call_args[] = {
          state_,
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
        // If the arguments came from an unboxed array, we have to put them
        // back in the array before splatting them.
        if(inline_args->from_unboxed_array()) {
          std::vector<Type*> types;
          types.push_back(ctx_->ptr_type("State"));
          types.push_back(ctx_->Int32Ty);

          FunctionType* ft = FunctionType::get(ctx_->ptr_type("Object"), types, true);
          Function* func = cast<Function>(
              ctx_->module()->getOrInsertFunction("rbx_create_array", ft));

          std::vector<Value*> outgoing_args;
          outgoing_args.push_back(state_);
          outgoing_args.push_back(cint(inline_args->size()));

          for(size_t i = 0; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }

          Value* ary =
            b().CreateCall(func, outgoing_args, "ary");

          Value* outargs2[] = {
            state_,
            cint(1),
            ary
          };

          Value* wrapped = b().CreateCall(func, outargs2, "splat_ary");
          stack_push(wrapped);
        } else {
          std::vector<Type*> types;
          types.push_back(ctx_->ptr_type("State"));
          types.push_back(CallFrameTy);
          types.push_back(ctx_->Int32Ty);

          FunctionType* ft = FunctionType::get(ctx_->ptr_type("Object"), types, true);
          Function* func = cast<Function>(
              ctx_->module()->getOrInsertFunction("rbx_cast_for_splat_block_arg_varargs", ft));

          std::vector<Value*> outgoing_args;
          outgoing_args.push_back(state_);
          outgoing_args.push_back(call_frame_);
          outgoing_args.push_back(cint(inline_args->size()));

          for(size_t i = 0; i < inline_args->size(); i++) {
            outgoing_args.push_back(inline_args->at(i));
          }

          Value* ary =
            b().CreateCall(func, outgoing_args, "ary");
          check_for_exception(ary);
          stack_push(ary);
        }
      } else {
        Signature sig(ctx_, ObjType);
        sig << StateTy;
        sig << CallFrameTy;
        sig << ptr_type("Arguments");

        Value* call_args[] = {
          state_,
          call_frame_,
          args_
        };

        Value* val = sig.call("rbx_cast_for_splat_block_arg", call_args, 3,
            "cfmba", b());
        check_for_exception(val);
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
            cint(0),
            cint(offset::StackVariables::parent)
          };

          Value* varscope = b().CreateLoad(
              b().CreateGEP(vars_, idx), "scope.parent");
          */

          Signature sig(ctx_, ObjType);
          sig << StateTy;
          sig << "CallFrame";
          sig << ObjType;
          sig << ctx_->Int32Ty;
          sig << ctx_->Int32Ty;

          Value* call_args[] = {
            state_,
            nfo->call_frame(),
            stack_top(),
            cint(depth),
            cint(index)
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
          cint(0),
          cint(offset::StackVariables::parent)
        };

        Value* gep = b().CreateGEP(vars_, idx, "parent_pos");

        Value* parent = b().CreateLoad(gep, "scope.parent");
        set_scope_local(parent, index);
        return;
      }
      */

      // Handle depth > 1

      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);
      types.push_back(ctx_->Int32Ty);
      types.push_back(ctx_->Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_set_local_depth", ft));

      Value* call_args[] = {
        state_,
        call_frame_,
        stack_pop(),
        cint(depth),
        cint(index)
      };

      Value* val = b().CreateCall(func, call_args, "sld");
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
            cint(0),
            cint(offset::StackVariables::parent)
          };

          Value* varscope = b().CreateLoad(
              b().CreateGEP(vars_, idx), "scope.parent");
              */

          Signature sig(ctx_, ObjType);
          sig << StateTy;
          sig << "CallFrame";
          sig << ctx_->Int32Ty;
          sig << ctx_->Int32Ty;

          Value* call_args[] = {
            state_,
            nfo->call_frame(),
            cint(depth),
            cint(index)
          };

          Value* val =
              sig.call("rbx_push_local_from", call_args, 4, "vs_uplocal", b());

          check_for_exception(val);
          stack_push(val);
        }

        return;
      }

      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ctx_->Int32Ty);
      types.push_back(ctx_->Int32Ty);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_local_depth", ft));

      Value* call_args[] = {
        state_,
        call_frame_,
        cint(depth),
        cint(index)
      };

      Value* val = b().CreateCall(func, call_args, "pld");
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
          cond, ctx_->IntPtrTy, "as_int");

      Value* anded = b().CreateAnd(i,
          clong(FALSE_MASK), "and");

      Value* cmp = b().CreateICmpNE(anded,
          clong(reinterpret_cast<long>(cFalse)), "is_true");

      BasicBlock* cont = new_block("continue");
      BasicBlock* bb = block_map_[ip].block;
      assert(bb);
      b().CreateCondBr(cmp, bb, cont);

      set_block(cont);
    }

    void visit_goto_if_false(opcode ip) {
      Value* cond = stack_pop();
      Value* i = b().CreatePtrToInt(
          cond, ctx_->IntPtrTy, "as_int");

      Value* anded = b().CreateAnd(i,
          clong(FALSE_MASK), "and");

      Value* cmp = b().CreateICmpEQ(anded,
          clong(reinterpret_cast<long>(cFalse)), "is_true");

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
      // statically! woo! ok, lets just emit the code for it here!
      if(ib && ib->machine_code()) {
        ctx_->set_inlined_block(true);

        JITMethodInfo* creator = ib->creation_scope();
        assert(creator);

        // Count the block against the policy size total
        inline_policy()->increase_size(ib->machine_code());

        // We inline unconditionally here, since we make the decision
        // wrt the block when we are considering inlining the send that
        // has the block on it.
        Inliner inl(ctx_, *this, count);

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

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << "Object";
      sig << ctx_->Int32Ty;
      sig << ObjArrayTy;

      Value* block_obj = b().CreateLoad(
          b().CreateConstGEP2_32(vars, 0, offset::StackVariables::block),
          "block");

      Value* call_args[] = {
        state_,
        call_frame_,
        block_obj,
        cint(count),
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

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << "Object";
      sig << ctx_->Int32Ty;
      sig << ObjArrayTy;

      Value* block_obj = b().CreateLoad(
          b().CreateConstGEP2_32(vars_, 0, offset::StackVariables::block),
          "block");

      Value* call_args[] = {
        state_,
        call_frame_,
        block_obj,
        cint(count),
        stack_objects(count + 1)
      };

      flush_ip();
      Value* val = sig.call("rbx_yield_splat", call_args, 5, "ys", b());
      stack_remove(count + 1);

      check_for_exception(val);
      stack_push(val);
    }

    void visit_check_interrupts() {

      BasicBlock* cont = new_block("continue");
      BasicBlock* interrupts = new_block("interrupts");
      BasicBlock* done = new_block("done");

      Value* idx_jit[] = {
        cint(0),
        cint(offset::State::vm_jit)
      };

      Value* vm_jit = b().CreateLoad(b().CreateGEP(state_, idx_jit), "vm_jit");

      Value* idx_check[] = {
        cint(0),
        cint(offset::VMJIT::check_local_interrupts)
      };

      Value* check_interrupts = b().CreateLoad(b().CreateGEP(vm_jit, idx_check),
                                               "check_interrupts");

      Value* check_global_interrupts = b().CreateLoad(check_global_interrupts_pos, "check_global_interrupts");
      Value* checkpoint = b().CreateOr(check_interrupts, check_global_interrupts, "or");

      Value* zero = ConstantInt::get(ctx_->Int8Ty, 0);
      Value* is_zero = b().CreateICmpEQ(checkpoint, zero, "needs_interrupts");

      create_conditional_branch(cont, interrupts, is_zero);

      set_block(cont);
      create_branch(done);

      set_block(interrupts);

      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_check_interrupts", ft));

      func->setDoesNotCapture(0);
      func->setDoesNotCapture(1);
      func->setDoesNotCapture(2);

      flush();

      Value* call_args[] = {
        state_,
        call_frame_
      };

      Value* ret = b().CreateCall(func, call_args, "ci");
      check_for_exception(ret, false);
      create_branch(done);

      set_block(done);
    }

    void emit_check_serial(opcode& index, opcode serial, const char* function) {

      CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&index);
      Value* call_site_ptr_const = b().CreateIntToPtr(
          clong(reinterpret_cast<uintptr_t>(call_site_ptr)),
          ptr_type(ptr_type("CallSite")), "cast_to_ptr");

      Value* call_site_const = b().CreateLoad(call_site_ptr_const, "cache_const");

      BasicBlock* mono_cache = new_block("mono_cache");
      BasicBlock* check_poly_cache = new_block("check_poly_cache");
      BasicBlock* poly_cache = new_block("mono_cache");
      BasicBlock* cont = new_block("cont");
      BasicBlock* fallback = new_block("fallback");
      BasicBlock* done = new_block("done");
      BasicBlock* mono_class_match = new_block("mono_class_match");

      Value* recv = stack_pop();

      Value* is_ref = check_is_reference(recv);
      create_conditional_branch(cont, fallback, is_ref);

      set_block(cont);

      Value* recv_class = reference_class(recv);

      Value* is_mono_cache = check_type_bits(call_site_const, rubinius::MonoInlineCache::type, "is_mono_cache");
      create_conditional_branch(mono_cache, check_poly_cache, is_mono_cache);

      set_block(mono_cache);

      Value* mono_const = b().CreateBitCast(call_site_const, ptr_type("MonoInlineCache"), "downcast");

      Value* receiver_class_idx[] = {
        cint(0),
        cint(offset::MonoInlineCache::receiver_class)
      };

      Value* receiver_class = b().CreateLoad(b().CreateGEP(mono_const, receiver_class_idx, "receiver_class"));
      Value* check_class = b().CreateICmpEQ(recv_class, receiver_class, "check_class");

      create_conditional_branch(mono_class_match, fallback, check_class);
      set_block(mono_class_match);

      Value* method_idx[] = {
        cint(0),
        cint(offset::MonoInlineCache::method)
      };
      Value* method = b().CreateLoad(b().CreateGEP(mono_const, method_idx, "method"));

      Value* executable_serial_idx[] = {
        cint(0),
        cint(offset::Executable::serial)
      };
      Value* executable_serial = b().CreateLoad(b().CreateGEP(method, executable_serial_idx, "executable_serial"));
      Value* check_serial = b().CreateICmpEQ(fixnum_strip(executable_serial), clong(serial), "check_serial");

      create_conditional_branch(done, fallback, check_serial);

      set_block(check_poly_cache);

      Value* is_poly_cache = check_type_bits(call_site_const, rubinius::PolyInlineCache::type, "is_inline_cache");
      create_conditional_branch(poly_cache, fallback, is_poly_cache);

      set_block(poly_cache);
      BasicBlock* blocks[cTrackedICHits];

      Value* poly_const = b().CreateBitCast(call_site_const, ptr_type("PolyInlineCache"), "downcast");

      for(int i = 0; i < cTrackedICHits; ++i) {
        Value* ice_idx[] = {
          cint(0),
          cint(offset::PolyInlineCache::entries),
          cint(i)
        };

        Value* ice = b().CreateLoad(b().CreateGEP(poly_const, ice_idx, "ice"));

        BasicBlock* has_ice = new_block("has_ice");
        BasicBlock* class_match = new_block("class_match");
        BasicBlock* next =        new_block("next");

        Value* check_ice = b().CreateICmpNE(ice, Constant::getNullValue(ctx_->ptr_type("InlineCacheEntry")), "check_ice");
        create_conditional_branch(has_ice, next, check_ice);

        set_block(has_ice);
        Value* receiver_class_idx[] = {
          cint(0),
          cint(offset::InlineCacheEntry::receiver_class)
        };
        Value* receiver_class = b().CreateLoad(b().CreateGEP(ice, receiver_class_idx, "receiver_class"));

        Value* check_class = b().CreateICmpEQ(recv_class, receiver_class, "check_class");

        create_conditional_branch(class_match, next, check_class);
        set_block(class_match);

        Value* method_idx[] = {
          cint(0),
          cint(offset::InlineCacheEntry::method)
        };
        Value* method = b().CreateLoad(b().CreateGEP(ice, method_idx, "method"));

        Value* executable_serial_idx[] = {
          cint(0),
          cint(offset::Executable::serial)
        };
        Value* executable_serial = b().CreateLoad(b().CreateGEP(method, executable_serial_idx, "executable_serial"));
        Value* check_serial = b().CreateICmpEQ(fixnum_strip(executable_serial), clong(serial), "check_serial");

        blocks[i] = current_block();
        create_conditional_branch(done, fallback, check_serial);


        set_block(next);
      }

      create_branch(fallback);

      set_block(fallback);

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "CallFrame";
      sig << "CallSite";
      sig << ctx_->Int32Ty;
      sig << "Object";


      Value* call_args[] = {
        state_,
        call_frame_,
        call_site_const,
        cint(serial),
        recv
      };

      sig.setDoesNotCapture(function, 2);
      Value* fallback_result = sig.call(function, call_args, 5, "cs", b());

      create_branch(done);
      set_block(done);

      PHINode* phi = b().CreatePHI(ObjType, 1 + cTrackedICHits, "constant");

      phi->addIncoming(constant(cTrue), mono_class_match);

      for(int i = 0; i < cTrackedICHits; ++i) {
        phi->addIncoming(constant(cTrue), blocks[i]);
      }

      phi->addIncoming(fallback_result, fallback);

      stack_push(phi);
    }

    void visit_check_serial(opcode& index, opcode serial) {
      emit_check_serial(index, serial, "rbx_check_serial");
    }

    void visit_check_serial_private(opcode& index, opcode serial) {
      emit_check_serial(index, serial, "rbx_check_serial_private");
    }

    void visit_push_my_offset(opcode i) {
      if(llvm_state()->config().jit_inline_debug) {
        ctx_->inline_log("inline slot read")
             << "offset: " << i << "\n";
      }

      Value* idx[] = {
        cint(0),
        cint(offset::StackVariables::self)
      };

      Value* pos = b().CreateGEP(vars_, idx, "self_pos");

      Value* self = b().CreateLoad(pos, "self");

      assert(i % sizeof(Object*) == 0);

      Value* cst = b().CreateBitCast(
          self,
          llvm::PointerType::getUnqual(ObjType), "obj_array");

      Value* idx2[] = {
        cint(i / sizeof(Object*))
      };

      pos = b().CreateGEP(cst, idx2, "field_pos");

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

        std::vector<Type*> types;
        types.push_back(StateTy);

        FunctionType* ft = FunctionType::get(ctx_->Int1Ty, types, false);
        Function* func = cast<Function>(
            module_->getOrInsertFunction("rbx_raising_exception", ft));

        Value* call_args[] = { state_ };
        Value* isit = b().CreateCall(func, call_args, "rae");

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

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        state_,
        call_frame_,
        stack_top()
      };

      sig.call("rbx_raise_return", call_args, 3, "raise_return", b());
      visit_reraise();
    }

    void visit_ensure_return() {
      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        state_,
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

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << ObjType;

      Value* call_args[] = {
        state_,
        call_frame_,
        stack_top()
      };

      sig.call("rbx_raise_break", call_args, 3, "raise_break", b());
      visit_reraise();
    }

    void visit_push_current_exception() {
      std::vector<Type*> types;

      types.push_back(StateTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_current_exception", ft));

      Value* call_args[] = { state_ };

      stack_push(b().CreateCall(func, call_args, "ce"));
    }

    void visit_clear_exception() {
      set_has_side_effects();

      std::vector<Type*> types;

      types.push_back(StateTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_clear_exception", ft));

      Value* call_args[] = { state_ };

      b().CreateCall(func, call_args);
    }

    void visit_push_exception_state() {
      std::vector<Type*> types;

      types.push_back(StateTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_push_exception_state", ft));

      Value* call_args[] = { state_ };

      stack_push(b().CreateCall(func, call_args));
    }

    void visit_restore_exception_state() {
      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(CallFrameTy);
      types.push_back(ObjType);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_restore_exception_state", ft));

      Value* call_args[] = { state_, call_frame_, stack_pop() };

      b().CreateCall(func, call_args);
    }

    void visit_find_const(opcode which) {
      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << ctx_->Int32Ty;
      sig << ObjType;

      Value* call_args[] = {
        state_,
        call_frame_,
        cint(which),
        stack_pop()
      };

      flush();

      Value* val = sig.call("rbx_find_const", call_args, 4, "constant", b());
      check_for_exception(val);
      stack_push(val);
    }

    void visit_find_const_fast(opcode& name) {
      BasicBlock* cont = 0;

      Value* cached_value = 0;
      BasicBlock* cached_block = 0;

      Value* under = stack_pop();

      ConstantCache** constant_cache_ptr = reinterpret_cast<ConstantCache**>(&name);
      ConstantCache* constant_cache = *constant_cache_ptr;

      Value* global_serial = b().CreateLoad(global_serial_pos, "global_serial");


      Value* cache_ptr = b().CreateIntToPtr(
        ConstantInt::get(context()->IntPtrTy, (reinterpret_cast<uintptr_t>(constant_cache_ptr))),
        ptr_type(ptr_type("ConstantCache")), "cast_to_ptr");

      Value* cache = b().CreateLoad(cache_ptr, "constant_cache");

      Value* constant_scope_pos_idx[] = {
        context()->cint(0),
        context()->cint(offset::ConstantCache::scope),
      };

      Value* constant_scope_pos = b().CreateGEP(cache,
          constant_scope_pos_idx, "constant_cache_pos");

      Value* constant_scope = b().CreateLoad(constant_scope_pos, "constant_scope");

      Value* frame_scope = b().CreateLoad(
          b().CreateConstGEP2_32(call_frame_, 0,
                                 offset::CallFrame::constant_scope, "scope_pos"), "frame_scope");

      Value* scope_cmp = b().CreateICmpEQ(constant_scope, frame_scope, "same_scope");

      BasicBlock* check_serial = new_block("check_serial");
      BasicBlock* check_under  = new_block("check_under");
      BasicBlock* use_cache    = new_block("use_cache");
      BasicBlock* use_call     = new_block("use_call");

      cont =      new_block("continue");

      b().CreateCondBr(scope_cmp, check_serial, use_call);

      set_block(check_serial);

      Value* serial_pos_idx[] = {
        context()->cint(0),
        context()->cint(offset::ConstantCache::serial),
      };

      Value* serial_pos = b().CreateGEP(cache,
          serial_pos_idx, "serial_pos");

      Value* current_serial = b().CreateLoad(serial_pos, "serial");

      Value* cache_cmp = b().CreateICmpEQ(global_serial, current_serial, "use_under");

      b().CreateCondBr(cache_cmp, check_under, use_call);

      set_block(check_under);

      Value* under_pos_idx[] = {
        context()->cint(0),
        context()->cint(offset::ConstantCache::under),
      };

      Value* under_pos = b().CreateGEP(cache,
          under_pos_idx, "value_pos");

      Value* cached_under = b().CreateBitCast(b().CreateLoad(under_pos, "cached_under"),
                                              ptr_type("Object"), "downcast");

      Value* under_cmp = b().CreateICmpEQ(cached_under, under, "use_cache");

      b().CreateCondBr(under_cmp, use_cache, use_call);

      set_block(use_cache);

      Value* value_pos_idx[] = {
        context()->cint(0),
        context()->cint(offset::ConstantCache::value),
      };

      Value* value_pos = b().CreateGEP(cache,
          value_pos_idx, "value_pos");

      cached_value = b().CreateLoad(value_pos, "cached_value");
      cached_block = b().GetInsertBlock();

      b().CreateBr(cont);

      set_block(use_call);

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << ptr_type(ptr_type("ConstantCache"));
      sig << ObjType;

      Value* call_args[] = {
        state_,
        call_frame_,
        constant(constant_cache_ptr, ptr_type(ptr_type("ConstantCache"))),
        under
      };

      flush();

      CallInst* ret = sig.call("rbx_find_const_fast", call_args, 4, "constant", b());
      ret->setOnlyReadsMemory();
      ret->setDoesNotThrow();
      check_for_exception(ret);

      BasicBlock* ret_block = b().GetInsertBlock();
      b().CreateBr(cont);
      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 2, "constant");
      phi->addIncoming(cached_value, cached_block);
      phi->addIncoming(ret, ret_block);

      stack_push(phi, type::KnownType::constant_cache(constant_cache));
    }

    void visit_instance_of() {
      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << ObjType;
      sig << ObjType;

      Value* top = stack_pop();
      Value* call_args[] = {
        state_,
        call_frame_,
        top,
        stack_pop()
      };

      Value* val = sig.call("rbx_instance_of", call_args, 4, "constant", b());
      stack_push(val);
    }

    void visit_kind_of() {
      Value* obj = stack_pop();
      Value* klass = stack_pop();
      stack_push(check_kind_of(obj, klass));
    }

    void visit_is_nil() {
      Value* cmp = b().CreateICmpEQ(stack_pop(),
          constant(cNil), "is_nil");
      Value* imm_value = b().CreateSelect(cmp, constant(cTrue),
          constant(cFalse), "select_bool");
      stack_push(imm_value);
    }

    void visit_make_array(opcode count) {
      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << ctx_->Int32Ty;
      sig << ObjArrayTy;

      Value* call_args[] = {
        state_,
        cint(count),
        stack_objects(count)
      };

      Value* val = sig.call("rbx_make_array", call_args, 3, "constant", b());

      stack_remove(count);
      stack_push(val);
    }

    void visit_meta_send_call(opcode& name, opcode count) {
      CallSite* call_site = reinterpret_cast<CallSite*>(name);

      if(call_site->type_id() == CallSiteType) {
        set_has_side_effects();

        Signature sig(ctx_, ObjType);

        sig << StateTy;
        sig << CallFrameTy;
        sig << "CallSite";
        sig << ctx_->Int32Ty;
        sig << ObjArrayTy;

        CallSite** call_site_ptr = reinterpret_cast<CallSite**>(&name);

        Value* call_site_ptr_const = b().CreateIntToPtr(
            clong(reinterpret_cast<uintptr_t>(call_site_ptr)),
            ptr_type(ptr_type("CallSite")), "cast_to_ptr");

        Value* call_site_const = b().CreateLoad(call_site_ptr_const, "cache_const");

        Value* call_args[] = {
          state_,
          call_frame_,
          call_site_const,
          cint(count),
          stack_objects(count + 1)
        };

        flush_ip();
        Value* val = sig.call("rbx_meta_send_call", call_args, 5, "constant", b());
        stack_remove(count+1);
        check_for_exception(val);
        stack_push(val);
      } else {
        invoke_call_site(name, count);
      }
    }

    void visit_passed_arg(opcode count) {
      count += machine_code()->post_args;

      if(called_args_ >= 0) {
        if((int)count < called_args_) {
          stack_push(constant(cTrue));
        } else {
          stack_push(constant(cFalse));
        }
      } else {
        Signature sig(ctx_, ObjType);

        sig << StateTy;
        sig << "Arguments";
        sig << ctx_->Int32Ty;

        Value* call_args[] = {
          state_,
          args_,
          cint(count)
        };

        Value* val = sig.call("rbx_passed_arg", call_args, 3, "pa", b());
        stack_push(val);
      }
    }

    void visit_passed_blockarg(opcode count) {
      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << "Arguments";
      sig << ctx_->Int32Ty;

      Value* call_args[] = {
        state_,
        args_,
        cint(count)
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

    void visit_push_type() {
      push_system_object(2);
    }

    void visit_push_mirror() {
      push_system_object(3);
    }

    void visit_push_ivar(opcode which) {
      Symbol* name = as<Symbol>(literal(which));

      BasicBlock* failure = new_block("visit_push_ivar_fallback");
      BasicBlock* cont = new_block("continue");

      // Reserve room on stack for result
      stack_push(constant(cNil));

      bool handled = false;

      if(Class* klass = try_as<Class>(info().self_class())) {

        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inline ivar read")
            << llvm_state()->symbol_debug_str(klass->module_name()) << "#"
            << llvm_state()->symbol_debug_str(name);
        }

        // Figure out if we should use the table ivar lookup or
        // the slot ivar lookup.

        Value* self = get_self();

        check_reference_class(self, klass, failure);

        TypeInfo* ti = klass->type_info();
        TypeInfo::Slots::iterator it = ti->slots.find(name->index());

        Value* ivar = 0;

        if(it != ti->slots.end()) {
          int offset = ti->slot_locations[it->second];
          ivar = get_object_slot(self, offset);
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << " (slot: " << it->second << ")";
          }
        } else {
          LookupTable* pii = klass->packed_ivar_info();
          if(!pii->nil_p()) {
            bool found = false;

            Fixnum* which = try_as<Fixnum>(pii->fetch(0, name, &found));
            if(found) {
              int index = which->to_native();
              int offset = sizeof(Object) + (sizeof(Object*) * index);
              Value* slot_val = get_object_slot(self, offset);

              Value* cmp = b().CreateICmpEQ(slot_val, constant(cUndef), "prune_undef");

              ivar = b().CreateSelect(cmp, constant(cNil), slot_val, "select ivar");

              if(llvm_state()->config().jit_inline_debug) {
                ctx_->log() << " (packed index: " << index << ", " << offset << ")";
              }
            }
          }
        }

        if(ivar) {
          stack_set_top(ivar);

          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "\n";
          }

          handled = true;
          create_branch(cont);
        } else {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << " (abort, using slow lookup)\n";
          }
        }
      }

      if(!handled) {
        create_branch(failure);
        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("slow ivar read")
            << llvm_state()->symbol_debug_str(name) << "\n";
        }
      }

      set_block(failure);
      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << ObjType;
      sig << ObjType;

      Value* self = get_self();

      Value* call_args[] = {
        state_,
        self,
        constant(as<Symbol>(literal(which)))
      };

      CallInst* val = sig.call("rbx_push_ivar", call_args, 3, "ivar", b());
      val->setOnlyReadsMemory();
      val->setDoesNotThrow();
      stack_set_top(val);

      create_branch(cont);
      set_block(cont);
    }

    void visit_set_ivar(opcode which) {
      Value* self = get_self();
      Value* ivar = stack_top();
      Symbol* name = as<Symbol>(literal(which));
      bool handled = false;

      BasicBlock* failure = new_block("visit_push_ivar_fallback");
      BasicBlock* cont = new_block("continue");

      set_has_side_effects();
      if(Class* klass = try_as<Class>(info().self_class())) {

        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inline ivar write")
            << llvm_state()->symbol_debug_str(name);
        }

        TypeInfo* ti = klass->type_info();
        TypeInfo::Slots::iterator it = ti->slots.find(name->index());

        check_reference_class(self, klass, failure);

        if(it != ti->slots.end()) {
          int field = it->second;
          int offset = ti->slot_locations[field];
          set_object_type_slot(self, field, offset, ti->slot_types[field], ivar);
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << " (slot: " << it->second << ")\n";
          }

          handled = true;
          create_branch(cont);
        } else {
          LookupTable* pii = klass->packed_ivar_info();
          if(!pii->nil_p()) {
            bool found = false;

            Fixnum* which = try_as<Fixnum>(pii->fetch(0, name, &found));
            if(found) {
              int index = which->to_native();
              int offset = sizeof(Object) + (sizeof(Object*) * index);

              set_object_slot(self, offset, ivar);

              if(llvm_state()->config().jit_inline_debug) {
                ctx_->log() << " (packed index: " << index << ", " << offset << ")\n";
              }
              handled = true;
              create_branch(cont);
            }
          }
        }

        if(!handled) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << " (abort, using slow write)\n";
          }
        }
      }

      if(!handled) {
        create_branch(failure);
        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("slow ivar write")
            << llvm_state()->symbol_debug_str(name) << "\n";
        }
      }

      set_block(failure);

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << "CallFrame";
      sig << ObjType;
      sig << ObjType;
      sig << ObjType;

      Value* call_args[] = {
        state_,
        call_frame_,
        self,
        constant(name),
        ivar
      };

      Value* ret = sig.call("rbx_set_ivar", call_args, 5, "ivar", b());
      check_for_exception(ret, false);

      create_branch(cont);
      set_block(cont);
    }

    void visit_push_my_field(opcode which) {
      Value* self = get_self();

      if(Class* klass = try_as<Class>(info().self_class())) {

        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inline field read");
          ctx_->log() << " (slot: " << which << ")\n";
        }

        TypeInfo* ti = klass->type_info();
        int offset = ti->slot_locations[which];
        stack_push(get_object_slot(self, offset));
      } else {

        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inline field read slow path");
          ctx_->log() << " (slot: " << which << ")\n";
        }
        Signature sig(ctx_, ObjType);

        sig << StateTy;
        sig << ObjType;
        sig << ctx_->Int32Ty;

        Value* call_args[] = {
          state_,
          self,
          cint(which)
        };

        Value* val = sig.call("rbx_push_my_field", call_args, 3, "field", b());
        check_for_exception(val);
        stack_push(val);
      }
    }

    void visit_store_my_field(opcode which) {
      set_has_side_effects();

      Value* self = get_self();
      Value* ivar = stack_top();

      if(Class* klass = try_as<Class>(info().self_class())) {

        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inline field write");
          ctx_->log() << " (slot: " << which << ")\n";
        }

        TypeInfo* ti = klass->type_info();
        int offset = ti->slot_locations[which];
        set_object_type_slot(self, which, offset, ti->slot_types[which], ivar);
      } else {

        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inline write slow path");
          ctx_->log() << " (slot: " << which << ")\n";
        }
        Signature sig(ctx_, ObjType);

        sig << StateTy;
        sig << ObjType;
        sig << ctx_->Int32Ty;
        sig << ObjType;

        Value* call_args[] = {
          state_,
          self,
          cint(which),
          ivar
        };

        sig.call("rbx_set_my_field", call_args, 4, "field", b());
      }
    }

    void visit_shift_array() {
      JITStackArgs* inline_args = incoming_args();
      if(inline_args && current_hint() == cHintLazyBlockArgs) {
        if(inline_args->size() <= (size_t)block_arg_shift_) {
          stack_push(constant(cNil));
        } else {
          stack_push(inline_args->at(block_arg_shift_));
          block_arg_shift_++;
        }
        return;
      }

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << ObjArrayTy;

      Value* call_args[] = {
        state_,
        call_frame_,
        stack_back_position(0)
      };

      Value* val = sig.call("rbx_shift_array", call_args, 3, "field", b());
      stack_push(val);
    }

    void visit_string_append() {
      set_has_side_effects();

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << ObjType;
      sig << ObjType;

      Value* val = stack_pop();

      Value* call_args[] = {
        state_,
        call_frame_,
        val,
        stack_pop()
      };

      Value* str = sig.call("rbx_string_append", call_args, 4, "string", b());
      stack_push(str);
    }

    void visit_string_build(opcode count) {
      set_has_side_effects();

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << CallFrameTy;
      sig << ctx_->Int32Ty;
      sig << ObjArrayTy;

      Value* call_args[] = {
        state_,
        call_frame_,
        cint(count),
        stack_objects(count)
      };

      Value* str = sig.call("rbx_string_build", call_args, 4, "string", b());
      stack_remove(count);

      check_for_exception(str);
      stack_push(str);
    }
  };
}
