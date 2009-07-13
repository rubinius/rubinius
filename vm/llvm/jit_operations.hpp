#ifndef RBX_LLVM_JIT_OPERATIONS
#define RBX_LLVM_JIT_OPERATIONS

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

using namespace llvm;

namespace rubinius {
  class JITOperations {
    llvm::Value* stack_;
    int sp_;
    int last_sp_;

  protected:
    VMMethod* vmm_;
    LLVMState* ls_;
    llvm::BasicBlock* block_;

    llvm::Module* module_;
    llvm::Function* function_;

    llvm::Value* vm_;
    llvm::Value* call_frame_;

    llvm::Value* zero_;
    llvm::Value* one_;

    InlinePolicy* inline_policy_;
    bool own_policy_;

  public:
    const llvm::Type* NativeIntTy;
    const llvm::Type* FixnumTy;
    const llvm::Type* IntPtrTy;
    const llvm::Type* ObjType;
    const llvm::Type* ObjArrayTy;
    const llvm::Type* Int31Ty;

    // Frequently used types
    const llvm::Type* VMTy;
    const llvm::Type* CallFrameTy;

    // Commonly used constants
    llvm::Value* Zero;
    llvm::Value* One;

  public:
    JITOperations(LLVMState* ls, VMMethod* vmm, llvm::Module* mod,
                  llvm::Value* stack, llvm::Value* call_frame,
                  llvm::BasicBlock* start, llvm::Function* func)
      : stack_(stack)
      , sp_(-1)
      , last_sp_(-1)
      , vmm_(vmm)
      , ls_(ls)
      , block_(start)
      , module_(mod)
      , function_(func)
      , call_frame_(call_frame)
      , inline_policy_(0)
      , own_policy_(false)
    {
      zero_ = ConstantInt::get(Type::Int32Ty, 0);
      one_ =  ConstantInt::get(Type::Int32Ty, 1);

#if __LP64__
      IntPtrTy = llvm::Type::Int64Ty;
      FixnumTy = llvvm::IntegerType::get(63);
#else
      IntPtrTy = llvm::Type::Int32Ty;
      FixnumTy = llvm::IntegerType::get(31);
#endif

      NativeIntTy = IntPtrTy;

      One = ConstantInt::get(NativeIntTy, 1);
      Zero = ConstantInt::get(NativeIntTy, 0);

      ObjType = ptr_type("Object");
      ObjArrayTy = PointerType::getUnqual(ObjType);

      Int31Ty = llvm::IntegerType::get(31);

      VMTy = ptr_type("VM");
      CallFrameTy = ptr_type("CallFrame");

      Function::arg_iterator input = function_->arg_begin();
      vm_ = input++;
    }

    virtual ~JITOperations() {
      if(inline_policy_ and own_policy_) delete inline_policy_;
    }

    void set_policy(InlinePolicy* policy) {
      inline_policy_ = policy;
    }

    void init_policy() {
      inline_policy_ = InlinePolicy::create_policy(vmm_);
      own_policy_ = true;
    }

    InlinePolicy* inline_policy() {
      return inline_policy_;
    }

    VMMethod* vmmethod() {
      return vmm_;
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

    InlineDecision should_inline_p(VMMethod* vmm) {
      if(inline_policy_) return inline_policy_->inline_p(vmm);
      return cNoPolicy;
    }

    static Value* cint(int num) {
      return ConstantInt::get(Type::Int32Ty, num);
    }

    // Type resolution and manipulation
    //
    const llvm::Type* ptr_type(std::string name) {
      std::string full_name = std::string("struct.rubinius::") + name;
      return PointerType::getUnqual(
          module_->getTypeByName(full_name.c_str()));
    }

    const llvm::Type* type(std::string name) {
      std::string full_name = std::string("struct.rubinius::") + name;
      return module_->getTypeByName(full_name.c_str());
    }

    Value* ptr_gep(Value* ptr, int which, const char* name, BasicBlock* block) {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, which)
      };

      return GetElementPtrInst::Create(ptr, idx, idx+2, name, block);
    }

    Value* upcast(Value* rec, const char* name) {
      const Type* type = ptr_type(name);

      return CastInst::Create(
          Instruction::BitCast,
          rec,
          type, "upcast", block_);
    }

    Value* check_type_bits(Value* obj, int type) {
      Value* flag_idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, 0)
      };

      Value* gep = create_gep(obj, flag_idx, 4, "flag_pos");
      Value* flags = create_load(gep, "flags");

      Value* mask = ConstantInt::get(Type::Int32Ty, (1 << 8) - 1);
      Value* obj_type = BinaryOperator::CreateAnd(flags, mask, "mask", block_);

      Value* tag = ConstantInt::get(Type::Int32Ty, type);
      return new ICmpInst(ICmpInst::ICMP_EQ, obj_type, tag, "is_tuple", block_);
    }

    Value* check_is_reference(Value* obj) {
      Value* mask = ConstantInt::get(IntPtrTy, TAG_REF_MASK);
      Value* zero = ConstantInt::get(IntPtrTy, TAG_REF);

      Value* lint = create_and(cast_int(obj), mask, "masked");
      return create_equal(lint, zero, "is_reference");
    }

    Value* reference_class(Value* obj) {
      Value* idx[] = { zero_, zero_, one_ };
      Value* gep = create_gep(obj, idx, 3, "class_pos");
      return create_load(gep, "ref_class");
    }

    Value* get_class_id(Value* cls) {
      Value* idx[] = { zero_, cint(3) };
      Value* gep = create_gep(cls, idx, 2, "class_id_pos");
      return create_load(gep, "class_id");
    }

    Value* check_is_symbol(Value* obj) {
      Value* mask = ConstantInt::get(IntPtrTy, TAG_SYMBOL_MASK);
      Value* zero = ConstantInt::get(IntPtrTy, TAG_SYMBOL);

      Value* lint = create_and(cast_int(obj), mask, "masked");
      return create_equal(lint, zero, "is_symbol");
    }

    Value* check_is_fixnum(Value* obj) {
      Value* mask = ConstantInt::get(IntPtrTy, TAG_FIXNUM_MASK);
      Value* zero = ConstantInt::get(IntPtrTy, TAG_FIXNUM);

      Value* lint = create_and(cast_int(obj), mask, "masked");
      return create_equal(lint, zero, "is_fixnum");
    }

    void verify_guard(Value* cmp, BasicBlock* failure) {
      BasicBlock* cont = new_block("guarded_body");
      create_conditional_branch(cont, failure, cmp);

      set_block(cont);

      failure->moveAfter(cont);
    }

    void check_class(Value* obj, Class* klass, BasicBlock* failure) {
      if(klass->instance_type()->to_native() == rubinius::Symbol::type) {
        verify_guard(check_is_symbol(obj), failure);
      } else if(klass->instance_type()->to_native() == rubinius::Fixnum::type) {
        verify_guard(check_is_fixnum(obj), failure);
      } else {
        check_reference_class(obj, klass->class_id(), failure);
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
      return block_;
    }

    BasicBlock* new_block(const char* name = "continue") {
      return BasicBlock::Create(name, function_);
    }

    void set_block(BasicBlock* bb) {
      block_ = bb;
    }

    // Stack manipulations
    //
    Value* stack_ptr(BasicBlock* block = NULL) {
      if(!block) block = block_;

      assert(sp_ >= 0 && sp_ < vmm_->stack_size);
      Value* idx = ConstantInt::get(Type::Int32Ty, sp_);
      return GetElementPtrInst::Create(stack_, &idx, &idx+1,
                                       "stack_pos", block);
    }

    void set_stack_ptr(Value* pos, BasicBlock* block = NULL) {
      abort();
    }

    void set_sp(int sp) {
      sp_ = sp;
      assert(sp_ >= -1 && sp_ < vmm_->stack_size);
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
      return ConstantInt::get(Type::Int32Ty, last_sp_);
    }

    Value* stack_position(int amount, BasicBlock* block = NULL) {
      if(!block) block = block_;

      int pos = sp_ + amount;
      assert(pos >= 0 && pos < vmm_->stack_size);

      Value* idx = ConstantInt::get(Type::Int32Ty, pos);

      Value* stack_pos = GetElementPtrInst::Create(
          stack_, &idx, &idx+1, "stack_pos", block);

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

    void stack_ptr_adjust(int amount, BasicBlock* block = NULL) {
      sp_ += amount;
      assert(sp_ >= -1 && sp_ < vmm_->stack_size);
    }

    void stack_remove(int count=1) {
      sp_ -= count;
      assert(sp_ >= -1 && sp_ < vmm_->stack_size);
    }

    void stack_push(Value* val, BasicBlock* block = NULL) {
      if(!block) block = block_;
      stack_ptr_adjust(1, block);
      Value* stack_pos = stack_ptr(block);

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

    llvm::Value* stack_top(BasicBlock* block = NULL) {
      if(!block) block = block_;
      return stack_back(0, block);
    }

    void stack_set_top(Value* val) {
      new StoreInst(val, stack_ptr(), false, block_);
    }

    llvm::Value* stack_pop(BasicBlock* block = NULL) {
      if(!block) block = block_;

      Value* val = stack_back(0, block);

      stack_ptr_adjust(-1, block);
      return val;
    }

    // Scope maintainence
    void flush_scope_to_heap(Value* vars) {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::vars_on_heap)
      };

      Value* pos = GetElementPtrInst::Create(vars, idx, idx + 2, "on_heap_pos", block_);

      Value* on_heap = new LoadInst(pos, "on_heap", block_);

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

      sig.call("rbx_flush_scope", call_args, 2, "", block_);

      create_branch(cont);

      set_block(cont);
    }

    // Constant creation
    //
    Value* constant(Object* obj, BasicBlock* block = NULL) {
      if(!block) block = block_;
      return CastInst::Create(
          Instruction::IntToPtr,
          ConstantInt::get(IntPtrTy, (intptr_t)obj),
          ObjType, "const_obj", block);
    }

    Value* constant(Object* obj, const Type* type, BasicBlock* block = NULL) {
      if(!block) block = block_;
      return CastInst::Create(
          Instruction::IntToPtr,
          ConstantInt::get(IntPtrTy, (intptr_t)obj),
          type, "const_of_type", block);
    }

    Value* ptrtoint(Value* ptr) {
      return CastInst::Create(
          Instruction::PtrToInt,
          ptr, IntPtrTy, "ptr2int", block_);
    }

    Value* subtract_pointers(Value* ptra, Value* ptrb) {
      Value* inta = ptrtoint(ptra);
      Value* intb = ptrtoint(ptrb);

      Value* sub = BinaryOperator::CreateSub(inta, intb, "ptr_diff", block_);

      Value* size_of = ConstantInt::get(IntPtrTy, sizeof(uintptr_t));

      return BinaryOperator::CreateSDiv(sub, size_of, "ptr_diff_adj", block_);
    }

    // numeric manipulations
    //
    Value* cast_int(Value* obj, BasicBlock* block = NULL) {
      if(!block) block = block_;

      return CastInst::Create(
          Instruction::PtrToInt,
          obj,
          IntPtrTy, "cast", block);
    }

    // Fixnum manipulations
    //
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

    Value* tag_strip32(Value* obj) {
      Value* i = CastInst::Create(
          Instruction::PtrToInt,
          obj, Type::Int32Ty, "as_int", block_);

      return BinaryOperator::CreateLShr(
          i, ConstantInt::get(Type::Int32Ty, 1),
          "lshr", block_);
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

    Value* nint(int val) {
      return ConstantInt::get(NativeIntTy, val);
    }

    Value* fixnum_strip(Value* obj) {
      Value* i = CastInst::Create(
          Instruction::PtrToInt,
          obj, NativeIntTy, "as_int", block_);

      return BinaryOperator::CreateLShr(i, One, "lshr", block_);
    }

    Value* as_obj(Value* val) {
      return CastInst::Create(
          Instruction::IntToPtr, val, ObjType, "as_obj", block_);
    }

    Value* check_if_fixnum(Value* val) {
      Value* fix_mask = ConstantInt::get(IntPtrTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(val);
      Value* masked = BinaryOperator::CreateAnd(lint, fix_mask, "masked", block_);

      return new ICmpInst(ICmpInst::ICMP_EQ, masked, fix_tag, "is_fixnum", block_);
    }

    // Tuple access
    Value* get_tuple_size(Value* tup) {
      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::tuple_full_size)
      };

      Value* pos = create_gep(tup, idx, 2, "table_size_pos");

      return create_load(pos, "table_size");
    }

    // Object access
    Value* get_object_slot(Value* obj, int offset) {
      assert(offset % sizeof(Object*) == 0);

      Value* cst = CastInst::Create(
          Instruction::BitCast,
          obj,
          PointerType::getUnqual(ObjType), "obj_array", block_);

      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, offset / sizeof(Object*))
      };

      Value* pos = create_gep(cst, idx2, 1, "field_pos");

      return create_load(pos, "field");
    }

    void set_object_slot(Value* obj, int offset, Value* val) {
      assert(offset % sizeof(Object*) == 0);

      Value* cst = CastInst::Create(
          Instruction::BitCast,
          obj,
          PointerType::getUnqual(ObjType), "obj_array", block_);

      Value* idx2[] = {
        ConstantInt::get(Type::Int32Ty, offset / sizeof(Object*))
      };

      Value* pos = create_gep(cst, idx2, 1, "field_pos");

      create_store(val, pos);
      write_barrier(obj, val);
    }

    // Utilities for creating instructions
    //
    GetElementPtrInst* create_gep(Value* rec, Value** idx, int count,
                                  const char* name) {
      return GetElementPtrInst::Create(rec, idx, idx+count, name, block_);
    }

    LoadInst* create_load(Value* ptr, const char* name = 0) {
      return new LoadInst(ptr, name, block_);
    }

    void create_store(Value* val, Value* ptr) {
      new StoreInst(val, ptr, false, block_);
    }

    ICmpInst* create_icmp(ICmpInst::Predicate kind, Value* left, Value* right,
                          const char* name) {
      return new ICmpInst(kind, left, right, name, block_);
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
      return BinaryOperator::CreateAnd(left, right, name, block_);
    }

    void create_conditional_branch(BasicBlock* if_true, BasicBlock* if_false, Value* cmp) {
      BranchInst::Create(if_true, if_false, cmp, block_);
    }

    void create_branch(BasicBlock* where) {
      BranchInst::Create(where, block_);
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

      Value* call_args[] = { vm_, obj, val };
      wb.call("rbx_write_barrier", call_args, 3, "", block_);
    }

    virtual void check_for_exception(llvm::Value* val) = 0;
  };
}

#endif
