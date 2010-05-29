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

#include <llvm/Value.h>
#include <llvm/BasicBlock.h>
#include <llvm/Function.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/CallingConv.h>

using namespace llvm;

namespace rubinius {

  namespace jit {
    class Context;
  }

  class JITOperations {
    llvm::Value* stack_;
    int sp_;
    int last_sp_;

    llvm::IRBuilder<> builder_;

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
    const llvm::Type* NativeIntTy;
    const llvm::Type* FixnumTy;
    const llvm::Type* ObjType;
    const llvm::Type* ObjArrayTy;

    // Frequently used types
    const llvm::Type* VMTy;
    const llvm::Type* CallFrameTy;

    // Commonly used constants
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

      ObjType = ptr_type("Object");
      ObjArrayTy = llvm::PointerType::getUnqual(ObjType);

      VMTy = ptr_type("VM");
      CallFrameTy = ptr_type("CallFrame");

      Function::arg_iterator input = function_->arg_begin();
      vm_ = input++;

      builder_.SetInsertPoint(start);
    }

    virtual ~JITOperations() {
      if(inline_policy_ and own_policy_) delete inline_policy_;
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

    void init_policy() {
      inline_policy_ = InlinePolicy::create_policy(vmmethod());
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

    VMMethod* vmmethod() {
      return method_info_.vmm;
    }

    Symbol* method_name() {
      return vmmethod()->name();
    }

    VMMethod* root_vmmethod() {
      if(method_info_.root) {
        return method_info_.root->vmm;
      } else {
        return vmmethod();
      }
    }

    std::vector<Value*>* incoming_args() {
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

    Value* cint(int num) {
      return ConstantInt::get(ls_->Int32Ty, num);
    }

    // Type resolution and manipulation
    //
    const llvm::Type* ptr_type(std::string name) {
      std::string full_name = std::string("struct.rubinius::") + name;
      return llvm::PointerType::getUnqual(
          module_->getTypeByName(full_name.c_str()));
    }

    const llvm::Type* type(std::string name) {
      std::string full_name = std::string("struct.rubinius::") + name;
      return module_->getTypeByName(full_name.c_str());
    }

    Value* ptr_gep(Value* ptr, int which, const char* name) {
      return b().CreateConstGEP2_32(ptr, 0, which, name);
    }

    Value* upcast(Value* rec, const char* name) {
      const Type* type = ptr_type(name);

      return b().CreateBitCast(rec, type, "upcast");
    }

    Value* downcast(Value* rec) {
      return b().CreateBitCast(rec, ptr_type("Object"), "downcast");
    }

    Value* check_type_bits(Value* obj, int type) {
      Value* word_idx[] = {
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, 0)
      };

      Value* gep = create_gep(obj, word_idx, 4, "word_pos");
      Value* word = create_load(gep, "flags");

      Value* mask = ConstantInt::get(ls_->Int32Ty, ((1 << 8) - 1) << 1);
      Value* flags = b().CreatePtrToInt(word, ls_->Int32Ty, "word2flags");
      Value* obj_type = b().CreateAnd(flags, mask, "mask");

      Value* tag = ConstantInt::get(ls_->Int32Ty, type << 1);

      return b().CreateICmpEQ(obj_type, tag, "is_tuple");
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
      Value* idx[] = { zero_, cint(4) };
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

    void check_class(Value* obj, Class* klass, BasicBlock* failure) {
      object_type type = (object_type)klass->instance_type()->to_native();

      switch(type) {
      case rubinius::Symbol::type:
        verify_guard(check_is_symbol(obj), failure);
        break;
      case rubinius::Fixnum::type:
        verify_guard(check_is_fixnum(obj), failure);
        break;
      case NilType:
        verify_guard(check_is_immediate(obj, Qnil), failure);
        break;
      case TrueType:
        verify_guard(check_is_immediate(obj, Qtrue), failure);
        break;
      case FalseType:
        verify_guard(check_is_immediate(obj, Qfalse), failure);
        break;
      default:
        check_reference_class(obj, klass->class_id(), failure);
        break;
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
      assert(which >= 0 && which < vmmethod()->stack_size);
      return b().CreateConstGEP1_32(stack_, which, "stack_pos");
    }

    Value* stack_ptr() {
      return stack_slot_position(sp_);
    }

    void set_sp(int sp) {
      sp_ = sp;
      assert(sp_ >= -1 && sp_ < vmmethod()->stack_size);
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
      assert(pos >= 0 && pos < vmmethod()->stack_size);

      return b().CreateConstGEP1_32(stack_, pos, "stack_pos");
    }

    Value* stack_back_position(int back) {
      return stack_position(-back);
    }

    Value* stack_objects(int count) {
      return stack_position(-(count - 1));
    }

    void stack_ptr_adjust(int amount) {
      sp_ += amount;
      assert(sp_ >= -1 && sp_ < vmmethod()->stack_size);
    }

    void stack_remove(int count=1) {
      sp_ -= count;
      assert(sp_ >= -1 && sp_ < vmmethod()->stack_size);
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
    }

    llvm::Value* stack_back(int back) {
      return b().CreateLoad(stack_back_position(back), "stack_load");
    }

    llvm::Value* stack_top() {
      return stack_back(0);
    }

    void stack_set_top(Value* val) {
      b().CreateStore(val, stack_ptr());
    }

    llvm::Value* stack_pop() {
      Value* val = stack_back(0);

      stack_ptr_adjust(-1);
      return val;
    }

    // Scope maintainence
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
      sig << "VM";
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

    Value* constant(void* obj, const Type* type) {
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
    Value* tag_strip(Value* obj, const Type* type = NULL) {
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
          i, ConstantInt::get(ls_->Int32Ty, 1),
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
        ConstantInt::get(ls_->Int32Ty, 0),
        ConstantInt::get(ls_->Int32Ty, offset::tuple_full_size)
      };

      Value* pos = create_gep(tup, idx, 2, "table_size_pos");

      return b().CreateIntCast(
          create_load(pos, "table_size"),
          NativeIntTy,
          true,
          "to_native_int");
    }

    // Object access
    Value* get_object_slot(Value* obj, int offset) {
      assert(offset % sizeof(Object*) == 0);

      Value* cst = b().CreateBitCast(
          obj,
          llvm::PointerType::getUnqual(ObjType), "obj_array");

      Value* idx2[] = {
        ConstantInt::get(ls_->Int32Ty, offset / sizeof(Object*))
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
        ConstantInt::get(ls_->Int32Ty, offset / sizeof(Object*))
      };

      Value* pos = create_gep(cst, idx2, 1, "field_pos");

      create_store(val, pos);
      write_barrier(obj, val);
    }

    // Utilities for creating instructions
    //
    GetElementPtrInst* create_gep(Value* rec, Value** idx, int count,
                                  const char* name) {
      return cast<GetElementPtrInst>(b().CreateGEP(rec, idx, idx+count, name));
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
      wb << VMTy;
      wb << ObjType;
      wb << ObjType;

      if(obj->getType() != ObjType) {
        obj = b().CreateBitCast(obj, ObjType, "casted");
      }

      Value* call_args[] = { vm_, obj, val };
      wb.call("rbx_write_barrier", call_args, 3, "", b());
    }

    virtual void check_for_exception(llvm::Value* val) = 0;
    virtual void propagate_exception() = 0;
  };
}

#endif
