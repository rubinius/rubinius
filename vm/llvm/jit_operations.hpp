#ifndef RBX_LLVM_JIT_OPERATIONS
#define RBX_LLVM_JIT_OPERATIONS

#include "config.h"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "inline_cache.hpp"

#include "llvm/offset.hpp"

#include "llvm/inline_policy.hpp"

#include "llvm/jit_context.hpp"
#include "llvm/types.hpp"

#include "llvm/method_info.hpp"
#include "llvm/jit_runtime.hpp"

#include <llvm/Value.h>
#include <llvm/BasicBlock.h>
#include <llvm/Function.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/CallingConv.h>

using namespace llvm;

namespace rubinius {

  class JITStackArgs;

  namespace jit {
    class Context;
  }

  struct ValueHint {
    int hint;
    llvm::MDNode* metadata;
    std::vector<Value*> data;
    type::KnownType known_type;

    ValueHint()
      : hint(0)
      , metadata(0)
      , known_type()
    {}
  };

  class JITOperations {
    llvm::Value* stack_;
    int sp_;
    int last_sp_;

    llvm::IRBuilder<> builder_;
    std::vector<ValueHint> hints_;

  protected:

    // The single Arguments object on the stack, plus positions into it
    // that we store the call info
    Value* out_args_;
    Value* out_args_name_;
    Value* out_args_recv_;
    Value* out_args_block_;
    Value* out_args_total_;
    Value* out_args_arguments_;
    Value* out_args_container_;

  protected:
    JITMethodInfo& method_info_;
    LLVMState* ls_;

    llvm::Module* module_;
    llvm::Function* function_;

    llvm::Value* vm_;
    llvm::Value* call_frame_;

    llvm::Value* zero_;
    llvm::Value* one_;

    InlinePolicy* inline_policy_;
    bool own_policy_;

    llvm::Value* valid_flag_;

  public:
    llvm::Type* NativeIntTy;
    llvm::Type* FixnumTy;
    llvm::Type* ObjType;
    llvm::Type* ObjArrayTy;

    // Frequently used types
    llvm::Type* StateTy;
    llvm::Type* CallFrameTy;

    // Commonly used constants
    llvm::Value* NegOne;
    llvm::Value* Zero;
    llvm::Value* One;

  public:
    JITOperations(LLVMState* ls, JITMethodInfo& info, llvm::BasicBlock* start)
      : stack_(info.stack())
      , sp_(-1)
      , last_sp_(-1)
      , builder_(ls->ctx())
      , method_info_(info)
      , ls_(ls)
      , module_(ls->module())
      , function_(info.function())
      , call_frame_(info.call_frame())
      , inline_policy_(0)
      , own_policy_(false)
    {
      zero_ = ConstantInt::get(ls_->Int32Ty, 0);
      one_ =  ConstantInt::get(ls_->Int32Ty, 1);

#ifdef IS_X8664
      FixnumTy = llvm::IntegerType::get(ls_->ctx(), 63);
#else
      FixnumTy = llvm::IntegerType::get(ls_->ctx(), 31);
#endif

      NativeIntTy = ls_->IntPtrTy;

      One = ConstantInt::get(NativeIntTy, 1);
      Zero = ConstantInt::get(NativeIntTy, 0);
      NegOne = ConstantInt::get(NativeIntTy, -1);

      ObjType = ptr_type("Object");
      ObjArrayTy = llvm::PointerType::getUnqual(ObjType);

      StateTy = ptr_type("State");
      CallFrameTy = ptr_type("CallFrame");

      Function::arg_iterator input = function_->arg_begin();
      vm_ = input++;

      builder_.SetInsertPoint(start);
    }

    virtual ~JITOperations() {
      if(inline_policy_ and own_policy_) delete inline_policy_;
    }

    void init_out_args() {
      out_args_ = info().out_args();

      out_args_name_ = ptr_gep(out_args_, 0, "out_args_name");
      out_args_recv_ = ptr_gep(out_args_, 1, "out_args_recv");
      out_args_block_= ptr_gep(out_args_, 2, "out_args_block");
      out_args_total_= ptr_gep(out_args_, 3, "out_args_total");
      out_args_arguments_ = ptr_gep(out_args_, 4, "out_args_arguments");
      out_args_container_ = ptr_gep(out_args_, offset::args_container,
                                    "out_args_container");
    }

    void setup_out_args(int args) {
      b().CreateStore(stack_back(args), out_args_recv_);
      b().CreateStore(constant(cNil), out_args_block_);
      b().CreateStore(cint(args),
                    out_args_total_);
      b().CreateStore(Constant::getNullValue(ptr_type("Tuple")),
                      out_args_container_);
      if(args > 0) {
        b().CreateStore(stack_objects(args), out_args_arguments_);
      }
    }

    Value* out_args() {
      return out_args_;
    }

    IRBuilder<>& b() { return builder_; }

    void set_valid_flag(llvm::Value* val) {
      valid_flag_ = val;
    }

    llvm::Value* valid_flag() {
      return valid_flag_;
    }

    void set_policy(InlinePolicy* policy) {
      inline_policy_ = policy;
    }

    void init_policy(LLVMState* state) {
      inline_policy_ = InlinePolicy::create_policy(state, machine_code());
      own_policy_ = true;
    }

    InlinePolicy* inline_policy() {
      return inline_policy_;
    }

    jit::Context& context() {
      return method_info_.context();
    }

    JITMethodInfo& info() {
      return method_info_;
    }

    MachineCode* machine_code() {
      return method_info_.machine_code;
    }

    Symbol* method_name() {
      return machine_code()->name();
    }

    MachineCode* root_machine_code() {
      if(method_info_.root) {
        return method_info_.root->machine_code;
      } else {
        return machine_code();
      }
    }

    JITStackArgs* incoming_args() {
      return method_info_.stack_args;
    }

    JITMethodInfo* root_method_info() {
      if(method_info_.root) {
        return method_info_.root;
      }

      return &method_info_;
    }

    LLVMState* state() {
      return ls_;
    }

    Value* vm() {
      return vm_;
    }

    Function* function() {
      return function_;
    }

    Value* call_frame() {
      return call_frame_;
    }

    llvm::Value* cint(int num) {
      return ls_->cint(num);
    }

    llvm::Value* clong(uintptr_t num) {
      return llvm::ConstantInt::get(ls_->IntPtrTy, num);
    }

    // Type resolution and manipulation
    //
    llvm::Type* ptr_type(std::string name) {
      std::string full_name = std::string("struct.rubinius::") + name;
      return llvm::PointerType::getUnqual(
          module_->getTypeByName(full_name));
    }

    llvm::Type* type(std::string name) {
      std::string full_name = std::string("struct.rubinius::") + name;
      return module_->getTypeByName(full_name);
    }

    Value* ptr_gep(Value* ptr, int which, const char* name) {
      return b().CreateConstGEP2_32(ptr, 0, which, name);
    }

    Value* upcast(Value* rec, const char* name) {
      Type* type = ptr_type(name);

      return b().CreateBitCast(rec, type, "upcast");
    }

    Value* downcast(Value* rec) {
      return b().CreateBitCast(rec, ptr_type("Object"), "downcast");
    }

    Value* check_type_bits(Value* obj, int type, const char* name = "is_type") {
      Value* word_idx[] = {
        zero_,
        zero_,
        zero_,
        zero_
      };

      if(obj->getType() != ObjType) {
        obj = b().CreateBitCast(obj, ObjType);
      }

      // This checks 2 things, not just the type bits. It also checks
      // that the inflated flag is 0, because if it's a 1, then the type
      // bits have nothing to do with the type.
      //
      // We don't handle checking the type bits in the inflated header also.

      Value* gep = create_gep(obj, word_idx, 4, "word_pos");
      Value* word = create_load(gep, "flags");
      Value* flags = b().CreatePtrToInt(word, ls_->Int64Ty, "word2flags");

      // 9 bits worth of mask
      Value* mask = ConstantInt::get(ls_->Int64Ty, ((1 << 9) - 1));
      Value* obj_type = b().CreateAnd(flags, mask, "mask");

      // Compare all 9 bits.
      Value* tag = ConstantInt::get(ls_->Int64Ty, type << 1);

      return b().CreateICmpEQ(obj_type, tag, name);
    }

    Value* check_is_reference(Value* obj) {
      Value* mask = ConstantInt::get(ls_->IntPtrTy, TAG_REF_MASK);
      Value* zero = ConstantInt::get(ls_->IntPtrTy, TAG_REF);

      Value* lint = create_and(cast_int(obj), mask, "masked");
      return create_equal(lint, zero, "is_reference");
    }

    Value* reference_class(Value* obj) {
      Value* idx[] = { zero_, zero_, one_ };
      Value* gep = create_gep(obj, idx, 3, "class_pos");
      return create_load(gep, "ref_class");
    }

    Value* get_class_id(Value* cls) {
      Value* idx[] = { zero_, cint(offset::class_class_id) };
      Value* gep = create_gep(cls, idx, 2, "class_id_pos");
      return create_load(gep, "class_id");
    }

    Value* check_is_symbol(Value* obj) {
      Value* mask = ConstantInt::get(ls_->IntPtrTy, TAG_SYMBOL_MASK);
      Value* zero = ConstantInt::get(ls_->IntPtrTy, TAG_SYMBOL);

      Value* lint = create_and(cast_int(obj), mask, "masked");
      return create_equal(lint, zero, "is_symbol");
    }

    Value* check_is_fixnum(Value* obj) {
      Value* mask = ConstantInt::get(ls_->IntPtrTy, TAG_FIXNUM_MASK);
      Value* zero = ConstantInt::get(ls_->IntPtrTy, TAG_FIXNUM);

      Value* lint = create_and(cast_int(obj), mask, "masked");
      return create_equal(lint, zero, "is_fixnum");
    }

    Value* check_is_immediate(Value* obj, Object* imm) {
      return create_equal(obj, constant(imm), "is_immediate");
    }

    Value* check_is_tuple(Value* obj) {
      return check_type_bits(obj, rubinius::Tuple::type);
    }

    void verify_guard(Value* cmp, BasicBlock* failure) {
      BasicBlock* cont = new_block("guarded_body");
      create_conditional_branch(cont, failure, cmp);

      set_block(cont);

      failure->moveAfter(cont);
    }

    type::KnownType check_class(Value* obj, Class* klass, BasicBlock* failure) {
      object_type type = (object_type)klass->instance_type()->to_native();

      switch(type) {
      case rubinius::Symbol::type:
        if(ls_->type_optz()) {
          type::KnownType kt = type::KnownType::extract(ls_, obj);

          if(kt.symbol_p()) {
            context().info("eliding guard: detected symbol");
            return kt;
          } else {
            verify_guard(check_is_symbol(obj), failure);
          }
        } else {
          verify_guard(check_is_symbol(obj), failure);
        }

        return type::KnownType::symbol();
      case rubinius::Fixnum::type:
        {
          if(ls_->type_optz()) {
            type::KnownType kt = type::KnownType::extract(ls_, obj);

            if(kt.static_fixnum_p()) {
              context().info("eliding guard: detected static fixnum");
              return kt;
            } else {
              verify_guard(check_is_fixnum(obj), failure);
            }
          } else {
            verify_guard(check_is_fixnum(obj), failure);
          }
        }
        return type::KnownType::fixnum();
      case NilType:
        verify_guard(check_is_immediate(obj, cNil), failure);
        return type::KnownType::nil();
      case TrueType:
        verify_guard(check_is_immediate(obj, cTrue), failure);
        return type::KnownType::true_();
      case FalseType:
        verify_guard(check_is_immediate(obj, cFalse), failure);
        return type::KnownType::false_();
      default:
        {
          type::KnownType kt = type::KnownType::extract(ls_, obj);

          if(kt.type_p()) {
            if(ls_->config().jit_inline_debug) {
              context().info_log("eliding guard") << "Type object used statically\n";
            }

            return kt;

          } else if(kt.class_id() == klass->class_id()) {
            if(ls_->config().jit_inline_debug) {
              context().info_log("eliding redundant guard")
                << "class " << ls_->symbol_debug_str(klass->module_name())
                << " (" << klass->class_id() << ")\n";
            }

            return kt;
          }

          check_reference_class(obj, klass->class_id(), failure);
          if(kind_of<SingletonClass>(klass)) {
            return type::KnownType::singleton_instance(klass->class_id());
          } else {
            return type::KnownType::instance(klass->class_id());
          }
        }
      }
    }

    void check_reference_class(Value* obj, int needed_id, BasicBlock* failure) {
      Value* is_ref = check_is_reference(obj);
      BasicBlock* cont = new_block("check_class_id");
      BasicBlock* body = new_block("correct_class");

      create_conditional_branch(cont, failure, is_ref);

      set_block(cont);

      Value* klass = reference_class(obj);
      Value* class_id = get_class_id(klass);

      Value* cmp = create_equal(class_id, cint(needed_id), "check_class_id");

      create_conditional_branch(body, failure, cmp);

      set_block(body);

      failure->moveAfter(body);
    }

    // BasicBlock management
    //
    BasicBlock* current_block() {
      return b().GetInsertBlock();
    }

    BasicBlock* new_block(const char* name = "continue") {
      return BasicBlock::Create(ls_->ctx(), name, function_);
    }

    void set_block(BasicBlock* bb) {
      // block_ = bb;
      builder_.SetInsertPoint(bb);
    }

    // Stack manipulations
    //
    Value* stack_slot_position(int which) {
      assert(which >= 0 && which < machine_code()->stack_size);
      return b().CreateConstGEP1_32(stack_, which, "stack_pos");
    }

    Value* stack_ptr() {
      return stack_slot_position(sp_);
    }

    void set_sp(int sp) {
      sp_ = sp;
      assert(sp_ >= -1 && sp_ < machine_code()->stack_size);
      hints_.clear();
    }

    void remember_sp() {
      last_sp_ = sp_;
    }

    void reset_sp() {
      sp_ = last_sp_;
    }

    int last_sp() {
      return last_sp_;
    }

    Value* last_sp_as_int() {
      return ConstantInt::get(NativeIntTy, last_sp_);
    }

    void flush_stack() { }

    Value* stack_position(int amount) {
      int pos = sp_ + amount;
      assert(pos >= 0 && pos < machine_code()->stack_size);

      return b().CreateConstGEP1_32(stack_, pos, "stack_pos");
    }

    Value* stack_back_position(int back) {
      return stack_position(-back);
    }

    void store_stack_back_position(int back, Value* val) {
      Value* pos = stack_back_position(back);
      clear_hint(-back);
      b().CreateStore(val, pos);
    }

    Value* stack_objects(int count) {
      return stack_position(-(count - 1));
    }

    void stack_ptr_adjust(int amount) {
      sp_ += amount;
      assert(sp_ >= -1 && sp_ < machine_code()->stack_size);
    }

    void stack_remove(int count=1) {
      sp_ -= count;
      assert(sp_ >= -1 && sp_ < machine_code()->stack_size);
    }

    void stack_push(Value* val) {
      stack_ptr_adjust(1);
      Value* stack_pos = stack_ptr();

      if(val->getType() == cast<llvm::PointerType>(stack_pos->getType())->getElementType()) {
        b().CreateStore(val, stack_pos);
      } else {
        Value* cst = b().CreateBitCast(
          val,
          ObjType, "casted");
        b().CreateStore(cst, stack_pos);
      }

      clear_hint();

      if(type::KnownType::has_hint(ls_, val)) {
        set_known_type(type::KnownType::extract(ls_, val));
      }
    }

    void stack_push(Value* val, type::KnownType kt) {
      kt.associate(ls_, val);

      stack_ptr_adjust(1);
      Value* stack_pos = stack_ptr();

      if(val->getType() == cast<llvm::PointerType>(stack_pos->getType())->getElementType()) {
        b().CreateStore(val, stack_pos);
      } else {
        Value* cst = b().CreateBitCast(
          val,
          ObjType, "casted");
        b().CreateStore(cst, stack_pos);
      }

      clear_hint();

      set_known_type(kt);
    }

    void set_hint(int hint) {
      if((int)hints_.size() <= sp_) {
        hints_.resize(sp_ + 1);
      }

      hints_[sp_].hint = hint;
    }

    void set_known_type(type::KnownType kt) {
      if((int)hints_.size() <= sp_) {
        hints_.resize(sp_ + 1);
      }

      hints_[sp_].known_type = kt;
    }

    void set_hint_metadata(llvm::MDNode* md) {
      if((int)hints_.size() <= sp_) {
        hints_.resize(sp_ + 1);
      }

      hints_[sp_].metadata = md;
    }

    int current_hint() {
      if(hints_.size() <= (size_t)sp_) {
        return 0;
      }

      return hints_[sp_].hint;
    }

    llvm::MDNode* current_hint_metadata() {
      if(hints_.size() <= (size_t)sp_) {
        return 0;
      }

      return hints_[sp_].metadata;
    }

    llvm::MDNode* hint_metadata(int back) {
      int pos = sp_ - back;

      if(hints_.size() <= (size_t)pos) {
        return 0;
      }

      return hints_[pos].metadata;
    }

    type::KnownType hint_known_type(int back) {
      int pos = sp_ - back;

      if(hints_.size() <= (size_t)pos) {
        return type::KnownType::unknown();
      }

      return hints_[pos].known_type;
    }

    ValueHint* current_hint_value() {
      if((int)hints_.size() <= sp_) {
        hints_.resize(sp_ + 1);
      }

      return &hints_[sp_];
    }

    void clear_hint() {
      if(hints_.size() > (size_t)sp_) {
        hints_[sp_].hint = 0;
        hints_[sp_].metadata = 0;
        hints_[sp_].known_type = type::KnownType::unknown();
      }
    }

    void clear_hint(int back) {
      int target = sp_ + back;
      if(hints_.size() > (size_t)target) {
        hints_[target].hint = 0;
        hints_[target].metadata = 0;
        hints_[target].known_type = type::KnownType::unknown();
      }
    }

    llvm::Value* stack_back(int back) {
      Instruction* I = b().CreateLoad(stack_back_position(back), "stack_load");
      type::KnownType kt = hint_known_type(back);
      kt.associate(ls_, I);

      return I;
    }

    llvm::Value* stack_top() {
      return stack_back(0);
    }

    void stack_set_top(Value* val) {
      b().CreateStore(val, stack_ptr());

      clear_hint();

      if(type::KnownType::has_hint(ls_, val)) {
        set_known_type(type::KnownType::extract(ls_, val));
      }
    }

    llvm::Value* stack_pop() {
      Value* val = stack_back(0);

      stack_ptr_adjust(-1);
      return val;
    }

    // Scope maintenance
    void flush_scope_to_heap(Value* vars) {
      Value* pos = b().CreateConstGEP2_32(vars, 0, offset::vars_on_heap,
                                     "on_heap_pos");

      Value* on_heap = b().CreateLoad(pos, "on_heap");

      Value* null = ConstantExpr::getNullValue(on_heap->getType());
      Value* cmp = create_not_equal(on_heap, null, "null_check");

      BasicBlock* do_flush = new_block("do_flush");
      BasicBlock* cont = new_block("continue");

      create_conditional_branch(do_flush, cont, cmp);

      set_block(do_flush);

      Signature sig(ls_, "Object");
      sig << "State";
      sig << "StackVariables";

      Value* call_args[] = { vm_, vars };

      sig.call("rbx_flush_scope", call_args, 2, "", b());

      create_branch(cont);

      set_block(cont);
    }

    // Constant creation
    //
    Value* constant(Object* obj) {
      return b().CreateIntToPtr(
          ConstantInt::get(ls_->IntPtrTy, (intptr_t)obj),
          ObjType, "const_obj");
    }

    Value* constant(void* obj, Type* type) {
      return b().CreateIntToPtr(
          ConstantInt::get(ls_->IntPtrTy, (intptr_t)obj),
          type, "const_of_type");
    }

    Value* ptrtoint(Value* ptr) {
      return b().CreatePtrToInt(ptr, ls_->IntPtrTy, "ptr2int");
    }

    Value* subtract_pointers(Value* ptra, Value* ptrb) {
      Value* inta = ptrtoint(ptra);
      Value* intb = ptrtoint(ptrb);

      Value* sub = b().CreateSub(inta, intb, "ptr_diff");

      Value* size_of = ConstantInt::get(ls_->IntPtrTy, sizeof(uintptr_t));

      return b().CreateSDiv(sub, size_of, "ptr_diff_adj");
    }

    // numeric manipulations
    //
    Value* cast_int(Value* obj) {
      return b().CreatePtrToInt(
          obj, NativeIntTy, "cast");
    }

    // Fixnum manipulations
    //
    Value* tag_strip(Value* obj, Type* type = NULL) {
      if(!type) type = FixnumTy;

      Value* i = b().CreatePtrToInt(
          obj, NativeIntTy, "as_int");

      Value* more = b().CreateLShr(
          i, ConstantInt::get(NativeIntTy, 1),
          "lshr");
      return b().CreateIntCast(
          more, type, true, "stripped");
    }

    Value* tag_strip32(Value* obj) {
      Value* i = b().CreatePtrToInt(
          obj, ls_->Int32Ty, "as_int");

      return b().CreateLShr(
          i, one_,
          "lshr");
    }

    Value* fixnum_to_native(Value* obj) {
      Value* i = b().CreatePtrToInt(
          obj, NativeIntTy, "as_int");

      return b().CreateLShr(
          i, ConstantInt::get(NativeIntTy, 1),
          "lshr");
    }

    Value* fixnum_tag(Value* obj) {
      Value* native_obj = b().CreateZExt(
          obj, NativeIntTy, "as_native_int");
      Value* one = ConstantInt::get(NativeIntTy, 1);
      Value* more = b().CreateShl(native_obj, one, "shl");
      Value* tagged = b().CreateOr(more, one, "or");

      return b().CreateIntToPtr(tagged, ObjType, "as_obj");
    }

    Value* nint(int val) {
      return ConstantInt::get(NativeIntTy, val);
    }

    Value* fixnum_strip(Value* obj) {
      Value* i = b().CreatePtrToInt(
          obj, NativeIntTy, "as_int");

      return b().CreateLShr(i, One, "lshr");
    }

    Value* as_obj(Value* val) {
      return b().CreateIntToPtr(val, ObjType, "as_obj");
    }

    Value* check_if_fixnum(Value* val) {
      Value* fix_mask = ConstantInt::get(ls_->IntPtrTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ls_->IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(val);
      Value* masked = b().CreateAnd(lint, fix_mask, "masked");

      return b().CreateICmpEQ(masked, fix_tag, "is_fixnum");
    }

    Value* check_if_fixnums(Value* val, Value* val2) {
      Value* fix_mask = ConstantInt::get(ls_->IntPtrTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ls_->IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(val);
      Value* rint = cast_int(val2);

      Value* anded = b().CreateAnd(lint, rint, "fixnums_anded");
      Value* masked = b().CreateAnd(anded, fix_mask, "masked");

      return b().CreateICmpEQ(masked, fix_tag, "is_fixnum");
    }

    // Tuple access
    Value* get_tuple_size(Value* tup) {
      Value* idx[] = {
        zero_,
        cint(offset::tuple_full_size)
      };

      Value* pos = create_gep(tup, idx, 2, "table_size_pos");

      Value* bytes = b().CreateIntCast(
                        create_load(pos, "table_size"),
                        NativeIntTy,
                        true,
                        "to_native_int");

      Value* header = ConstantInt::get(NativeIntTy, sizeof(Tuple));
      Value* body_bytes = b().CreateSub(bytes, header);

      Value* ptr_size = ConstantInt::get(NativeIntTy, sizeof(Object*));
      return b().CreateSDiv(body_bytes, ptr_size);
    }

    // Object access
    Value* get_object_slot(Value* obj, int offset) {
      assert(offset % sizeof(Object*) == 0);

      Value* cst = b().CreateBitCast(
          obj,
          llvm::PointerType::getUnqual(ObjType), "obj_array");

      Value* idx2[] = {
        cint(offset / sizeof(Object*))
      };

      Value* pos = create_gep(cst, idx2, 1, "field_pos");

      return create_load(pos, "field");
    }

    void set_object_slot(Value* obj, int offset, Value* val) {
      assert(offset % sizeof(Object*) == 0);

      Value* cst = b().CreateBitCast(
          obj,
          llvm::PointerType::getUnqual(ObjType), "obj_array");

      Value* idx2[] = {
        cint(offset / sizeof(Object*))
      };

      Value* pos = create_gep(cst, idx2, 1, "field_pos");

      create_store(val, pos);
      write_barrier(obj, val);
    }

    // Utilities for creating instructions
    //
    GetElementPtrInst* create_gep(Value* rec, Value** idx, int count,
                                  const char* name) {
      return cast<GetElementPtrInst>(b().CreateGEP(rec, ArrayRef<Value*>(idx, count), name));
    }

    LoadInst* create_load(Value* ptr, const char* name = "") {
      return b().CreateLoad(ptr, name);
    }

    void create_store(Value* val, Value* ptr) {
      b().CreateStore(val, ptr);
    }

    ICmpInst* create_icmp(ICmpInst::Predicate kind, Value* left, Value* right,
                          const char* name) {
      return cast<ICmpInst>(b().CreateICmp(kind, left, right, name));
    }

    ICmpInst* create_equal(Value* left, Value* right, const char* name) {
      return create_icmp(ICmpInst::ICMP_EQ, left, right, name);
    }

    ICmpInst* create_not_equal(Value* left, Value* right, const char* name) {
      return create_icmp(ICmpInst::ICMP_NE, left, right, name);
    }

    ICmpInst* create_less_than(Value* left, Value* right, const char* name) {
      return create_icmp(ICmpInst::ICMP_SLT, left, right, name);
    }

    Value* create_and(Value* left, Value* right, const char* name) {
      return b().CreateAnd(left, right, name);
    }

    void create_conditional_branch(BasicBlock* if_true, BasicBlock* if_false, Value* cmp) {
      b().CreateCondBr(cmp, if_true, if_false);
    }

    void create_branch(BasicBlock* where) {
      b().CreateBr(where);
    }

    void write_barrier(Value* obj, Value* val) {
      Signature wb(ls_, ObjType);
      wb << StateTy;
      wb << ObjType;
      wb << ObjType;

      if(obj->getType() != ObjType) {
        obj = b().CreateBitCast(obj, ObjType, "casted");
      }

      Value* call_args[] = { vm_, obj, val };
      wb.call("rbx_write_barrier", call_args, 3, "", b());
    }

    void call_debug_spot(int spot) {
      Signature sig(ls_, ObjType);
      sig << ls_->Int32Ty;

      Value* call_args[] = { cint(spot) };

      sig.call("rbx_jit_debug_spot", call_args, 1, "", b());
    }

    void call_debug_spot(Value* val) {
      Signature sig(ls_, ObjType);
      sig << val->getType();

      Value* call_args[] = { val };

      sig.call("rbx_jit_debug_spot", call_args, 1, "", b());
    }

    virtual void check_for_exception(llvm::Value* val, bool pass_top=true) = 0;
    virtual void propagate_exception() = 0;
  };
}

#endif
