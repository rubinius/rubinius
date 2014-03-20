#ifndef RBX_LLVM_JIT_OPERATIONS
#define RBX_LLVM_JIT_OPERATIONS

#include "config.h"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/byte_array.hpp"
#include "builtin/regexp.hpp"
#include "builtin/constant_cache.hpp"
#include "object_utils.hpp"

#include "llvm/offset.hpp"

#include "llvm/inline_policy.hpp"

#include "llvm/jit_context.hpp"
#include "llvm/jit_builder.hpp"
#include "llvm/types.hpp"

#include "llvm/jit_context.hpp"
#include "llvm/method_info.hpp"
#include "llvm/jit_runtime.hpp"

#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/Value.h>
#elif RBX_LLVM_API_VER >= 302
#include <llvm/Value.h>
#endif
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#else
#include <llvm/BasicBlock.h>
#include <llvm/Function.h>
#endif
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/IRBuilder.h>
#elif RBX_LLVM_API_VER >= 302
#include <llvm/IRBuilder.h>
#else
#include <llvm/Support/IRBuilder.h>
#endif
#if RBX_LLVM_API_VER >= 303
#include <llvm/IR/CallingConv.h>
#else
#include <llvm/CallingConv.h>
#endif

using namespace llvm;

namespace rubinius {

  class JITStackArgs;
  class Context;

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

    IRBuilder builder_;
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
    Context* ctx_;

    llvm::Module* module_;
    llvm::Function* function_;

    llvm::Value* state_;
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

    llvm::Value* object_memory_mark_pos_;

  public:
    JITOperations(jit::Builder* builder, JITMethodInfo& info, llvm::BasicBlock* start)
      : stack_(info.stack())
      , sp_(-1)
      , last_sp_(-1)
      , builder_(builder->ctx_->llvm_context(), llvm::ConstantFolder(), IRBuilderInserterWithDebug(builder))
      , method_info_(info)
      , ctx_(builder->ctx_)
      , module_(builder->ctx_->module())
      , function_(info.function())
      , call_frame_(info.call_frame())
      , inline_policy_(0)
      , own_policy_(false)
    {
      zero_ = ConstantInt::get(ctx_->Int32Ty, 0);
      one_ =  ConstantInt::get(ctx_->Int32Ty, 1);

#ifdef IS_X8664
      FixnumTy = llvm::IntegerType::get(ctx_->llvm_context(), 63);
#else
      FixnumTy = llvm::IntegerType::get(ctx_->llvm_context(), 31);
#endif

      NativeIntTy = ctx_->IntPtrTy;

      One = ConstantInt::get(NativeIntTy, 1);
      Zero = ConstantInt::get(NativeIntTy, 0);
      NegOne = ConstantInt::get(NativeIntTy, -1);

      object_memory_mark_pos_ = b().CreateIntToPtr(
        clong((intptr_t)llvm_state()->shared().object_memory_mark_address()),
        llvm::PointerType::getUnqual(ctx_->Int32Ty), "cast_to_intptr");

      ObjType = ptr_type("Object");
      ObjArrayTy = llvm::PointerType::getUnqual(ObjType);

      StateTy = ptr_type("State");
      CallFrameTy = ptr_type("CallFrame");

      Function::arg_iterator input = function_->arg_begin();
      state_ = input++;

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
      out_args_container_ = ptr_gep(out_args_, offset::Arguments::argument_container,
                                    "out_args_container");
    }

    void setup_out_args(Symbol* name, int args) {
      b().CreateStore(stack_back(args), out_args_recv_);
      b().CreateStore(constant(name, ptr_type("Symbol")), out_args_name_);
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

    IRBuilder& b() { return builder_; }

    void set_valid_flag(llvm::Value* val) {
      valid_flag_ = val;
    }

    llvm::Value* valid_flag() {
      return valid_flag_;
    }

    void set_policy(InlinePolicy* policy) {
      inline_policy_ = policy;
    }

    void init_policy(Context* ctx) {
      inline_policy_ = InlinePolicy::create_policy(ctx, machine_code());
      own_policy_ = true;
    }

    InlinePolicy* inline_policy() {
      return inline_policy_;
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

    Context* context() {
      return ctx_;
    }

    LLVMState* llvm_state() {
      return ctx_->llvm_state();
    }

    Value* state() {
      return state_;
    }

    Function* function() {
      return function_;
    }

    Value* call_frame() {
      return call_frame_;
    }

    llvm::Constant* cint(int num) {
      return ctx_->cint(num);
    }

    llvm::Value* clong(uintptr_t num) {
      return ctx_->clong(num);
    }

    llvm::Type* ptr_type(llvm::Type* type) {
      return ctx_->ptr_type(type);
    }

    llvm::Type* ptr_type(std::string name) {
      return ctx_->ptr_type(name);
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

    Value* object_flags(Value* obj) {
      Value* word_idx[] = {
        zero_,
        zero_,
        zero_,
        zero_
      };

      if(obj->getType() != ObjType) {
        obj = b().CreateBitCast(obj, ObjType);
      }

      Value* gep = b().CreateGEP(obj, ArrayRef<Value*>(word_idx, 4), "word_pos");
      Value* word = create_load(gep, "flags");
      return b().CreatePtrToInt(word, ctx_->Int64Ty, "word2flags");
    }

    Value* check_type_bits(Value* obj, int type, const char* name = "is_type") {

      Value* flags = object_flags(obj);

      // 8 bits worth of mask
      Value* mask = ConstantInt::get(ctx_->Int64Ty, ((1 << (OBJECT_FLAGS_OBJ_TYPE + 1)) - 1));
      Value* obj_type = b().CreateAnd(flags, mask, "mask");

      // Compare all 8 bits.
      Value* tag = ConstantInt::get(ctx_->Int64Ty, type);

      return b().CreateICmpEQ(obj_type, tag, name);
    }

    Value* check_is_reference(Value* obj) {
      Value* mask = ConstantInt::get(ctx_->IntPtrTy, TAG_REF_MASK);
      Value* zero = ConstantInt::get(ctx_->IntPtrTy, TAG_REF);

      Value* lint = create_and(cast_int(obj), mask, "masked");
      return create_equal(lint, zero, "is_reference");
    }

    Value* check_header_bit(Value* obj, BasicBlock* failure, int bit) {
      Value* is_ref = check_is_reference(obj);
      BasicBlock* cont = new_block("reference");
      BasicBlock* positive = new_block("positive");
      BasicBlock* negative = new_block("negative");
      BasicBlock* done     = new_block("done");

      create_conditional_branch(cont, failure, is_ref);

      set_block(cont);

      Value* flags = object_flags(obj);
      Value* mask = ConstantInt::get(ctx_->Int64Ty, 1 << bit);

      Value* bit_obj = b().CreateAnd(flags, mask, "mask");
      Value* is_bit  = b().CreateICmpEQ(bit_obj, mask, "is_bit");

      create_conditional_branch(positive, negative, is_bit);
      set_block(positive);
      create_branch(done);

      set_block(negative);
      create_branch(done);

      set_block(done);

      PHINode* phi = b().CreatePHI(ObjType, 2, "equal_value");
      phi->addIncoming(constant(cTrue), positive);
      phi->addIncoming(constant(cFalse), negative);

      return phi;
    }

    Value* get_header_value(Value* recv, int header, const char* fallback) {
      BasicBlock* done = new_block("done");
      BasicBlock* check = new_block("check");
      BasicBlock* failure = new_block("failure");

      create_branch(check);
      set_block(check);

      Value* flag_res = check_header_bit(recv, failure, header);

      check = current_block();

      create_branch(done);
      set_block(failure);

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "Object";

      Function* func = sig.function(fallback);
      func->setDoesNotAlias(0); // return value

      Value* call_args[] = { state_, recv };
      CallInst* call_res = sig.call(fallback, call_args, 2, "result", b());
      call_res->setOnlyReadsMemory();
      call_res->setDoesNotThrow();

      create_branch(done);
      set_block(done);

      PHINode* res = b().CreatePHI(ObjType, 2, "result");
      res->addIncoming(flag_res, check);
      res->addIncoming(call_res, failure);

      return res;
    }

    void check_is_frozen(Value* obj) {

      Value* is_ref = check_is_reference(obj);
      BasicBlock* done = new_block("done");
      BasicBlock* cont = new_block("reference");
      BasicBlock* failure = new_block("use_call");

      create_conditional_branch(cont, done, is_ref);

      set_block(cont);

      Value* flags = object_flags(obj);
      Value* mask = ConstantInt::get(ctx_->Int64Ty, 1 << OBJECT_FLAGS_FROZEN);

      Value* frozen_obj = b().CreateAnd(flags, mask, "mask");

      Value* not_frozen  = b().CreateICmpEQ(frozen_obj, ConstantInt::get(ctx_->Int64Ty, 0), "not_frozen");
      create_conditional_branch(done, failure, not_frozen);

      set_block(failure);

      Signature sig(ctx_, "Object");

      sig << "State";
      sig << "CallFrame";
      sig << "Object";

      Value* call_args[] = { state_, call_frame_, stack_top() };

      CallInst* res = sig.call("rbx_check_frozen", call_args, 3, "", b());
      res->setOnlyReadsMemory();
      res->setDoesNotThrow();

      check_for_exception(res, false);

      b().CreateBr(done);
      set_block(done);
    }

    Value* reference_class(Value* obj) {
      Value* idx[] = { zero_, zero_, one_ };
      Value* gep = create_gep(obj, idx, 3, "class_pos");
      return create_load(gep, "ref_class");
    }

    Value* get_class_id(Value* cls) {
      Value* idx[] = { zero_, cint(offset::Class::class_id) };
      Value* gep = create_gep(cls, idx, 2, "class_id_pos");
      return create_load(gep, "class_id");
    }

    Value* get_serial_id(Value* cls) {
      Value* idx[] = { zero_, cint(offset::Class::serial_id) };
      Value* gep = create_gep(cls, idx, 2, "serial_id_pos");
      return create_load(gep, "serial_id");
    }

    Value* check_is_symbol(Value* obj) {
      Value* mask = ConstantInt::get(ctx_->IntPtrTy, TAG_SYMBOL_MASK);
      Value* zero = ConstantInt::get(ctx_->IntPtrTy, TAG_SYMBOL);

      Value* lint = create_and(cast_int(obj), mask, "masked");
      return create_equal(lint, zero, "is_symbol");
    }

    Value* check_is_fixnum(Value* obj) {
      Value* mask = ConstantInt::get(ctx_->IntPtrTy, TAG_FIXNUM_MASK);
      Value* zero = ConstantInt::get(ctx_->IntPtrTy, TAG_FIXNUM);

      Value* lint = create_and(cast_int(obj), mask, "masked");
      return create_equal(lint, zero, "is_fixnum");
    }

    Value* check_is_immediate(Value* obj, Object* imm) {
      return create_equal(obj, constant(imm), "is_immediate");
    }

    Value* check_is_tuple(Value* obj) {
      return check_type_bits(obj, rubinius::Tuple::type);
    }

    Value* check_is_bytearray(Value* obj) {
      return check_type_bits(obj, rubinius::ByteArray::type);
    }

    Value* check_is_matchdata(Value* obj) {
      return check_type_bits(obj, rubinius::MatchData::type);
    }

    Value* check_is_regexp(Value* obj) {
      return check_type_bits(obj, rubinius::Regexp::type);
    }

    Value* check_is_class(Value* obj) {
      return check_type_bits(obj, rubinius::Class::type);
    }

    void check_direct_class(Value* obj, Value* klass, BasicBlock* success, BasicBlock* failure, bool allow_integer = false) {

      BasicBlock* verify_class_block = new_block("class_verified_block");
      BasicBlock* reference_block = new_block("reference_block");
      BasicBlock* check_symbol_block = new_block("check_symbol_block");
      BasicBlock* is_symbol_block = new_block("is_symbol_block");
      BasicBlock* check_fixnum_block = new_block("check_fixnum_block");
      BasicBlock* is_fixnum_block = new_block("is_fixnum_block");
      BasicBlock* is_integer_block = new_block("is_integer_block");

      BasicBlock* check_nil_block = new_block("check_nil_block");
      BasicBlock* is_nil_block = new_block("is_nil_block");
      BasicBlock* check_true_block = new_block("check_true_block");
      BasicBlock* is_true_block = new_block("is_true_block");
      BasicBlock* check_false_block = new_block("check_false_block");
      BasicBlock* is_false_block = new_block("is_false_block");

      Value* is_ref = check_is_reference(obj);

      create_conditional_branch(reference_block, verify_class_block, is_ref);
      set_block(reference_block);

      Value* obj_klass = b().CreateBitCast(reference_class(obj), ptr_type("Object"), "downcast");
      Value* check_class = create_equal(obj_klass, klass, "check_class");

      create_conditional_branch(success, failure, check_class);

      set_block(verify_class_block);

      Value* is_class = check_is_class(klass);
      create_conditional_branch(check_symbol_block, failure, is_class);
      set_block(check_symbol_block);

      Value* cast_klass = b().CreateBitCast(klass, ptr_type("Class"), "upcast");
      Value* class_id = get_class_id(cast_klass);

      Value* is_symbol = check_is_symbol(obj);
      create_conditional_branch(is_symbol_block, check_fixnum_block, is_symbol);
      set_block(is_symbol_block);
      Value* is_symbol_class = create_equal(class_id, cint(llvm_state()->symbol_class_id()), "check_class_id");
      create_conditional_branch(success, failure, is_symbol_class);

      set_block(check_fixnum_block);
      Value* is_fixnum = check_is_fixnum(obj);
      create_conditional_branch(is_fixnum_block, check_nil_block, is_fixnum);
      set_block(is_fixnum_block);
      Value* is_fixnum_class = create_equal(class_id, cint(llvm_state()->fixnum_class_id()), "check_class_id");

      if(allow_integer) {
        create_conditional_branch(success, is_integer_block, is_fixnum_class);
        set_block(is_integer_block);
        Value* is_integer_class = create_equal(class_id, cint(llvm_state()->integer_class_id()), "check_class_id");
        create_conditional_branch(success, failure, is_integer_class);
      } else {
        create_conditional_branch(success, failure, is_fixnum_class);
      }

      set_block(check_nil_block);
      Value* is_nil = check_is_immediate(obj, cNil);
      create_conditional_branch(is_nil_block, check_true_block, is_nil);
      set_block(is_nil_block);
      Value* is_nil_class = create_equal(class_id, cint(llvm_state()->nil_class_id()), "check_class_id");
      create_conditional_branch(success, failure, is_nil_class);

      set_block(check_true_block);
      Value* is_true = check_is_immediate(obj, cTrue);
      create_conditional_branch(is_true_block, check_false_block, is_true);
      set_block(is_true_block);
      Value* is_true_class = create_equal(class_id, cint(llvm_state()->true_class_id()), "check_class_id");
      create_conditional_branch(success, failure, is_true_class);

      set_block(check_false_block);
      Value* is_false = check_is_immediate(obj, cFalse);
      create_conditional_branch(is_false_block, failure, is_false);
      set_block(is_false_block);
      Value* is_false_class = create_equal(class_id, cint(llvm_state()->false_class_id()), "check_class_id");
      create_conditional_branch(success, failure, is_false_class);
    }

    Value* check_kind_of(Value* obj, Value* check_klass) {
      type::KnownType kt = type::KnownType::extract(ctx_, check_klass);

      BasicBlock* cont = new_block("continue");
      BasicBlock* immediate_positive_block = 0;
      BasicBlock* immediate_negative_block = 0;
      Class* klass = 0;
      BasicBlock* use_call  = new_block("use_call");
      BasicBlock* positive  = new_block("positive");
      BasicBlock* negative  = new_block("negative");

      uint32_t class_id = 0;

      if(kt.class_p()) {
        class_id = kt.class_id();
      }

      if(kt.constant_cache_p()) {
        ConstantCache* constant_cache = kt.constant_cache();
        klass = try_as<Class>(constant_cache->value());
        if(klass) {
          class_id = klass->class_id();
        }
      }

      if(class_id) {
        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inlining") << "direct class used for kind_of ";
        }

        BasicBlock* use_cache = new_block("use_cache");

        type::KnownType recv_type = type::KnownType::extract(ctx_, obj);
        uint32_t recv_class_id = recv_type.class_id();

        if(recv_type.instance_p() && class_id == recv_class_id) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(eliding because of statically known match)\n";
          }
          create_branch(positive);
        } else if(class_id == llvm_state()->fixnum_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against Fixnum)\n";
          }
          Value* is_fixnum = check_is_fixnum(obj);
          create_conditional_branch(positive, negative, is_fixnum);
        } else if(class_id == llvm_state()->integer_class_id() ||
                  class_id == llvm_state()->numeric_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against Integer / Numeric)\n";
          }
          Value* is_fixnum = check_is_fixnum(obj);
          create_conditional_branch(positive, use_call, is_fixnum);
        } else if(class_id == llvm_state()->bignum_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against Bignum)\n";
          }
          Value* is_ref = check_is_reference(obj);
          create_conditional_branch(use_cache, negative, is_ref);
          set_block(use_cache);

          Value* is_type = check_type_bits(obj, BignumType);
          create_conditional_branch(positive, negative, is_type);
        } else if(class_id == llvm_state()->float_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against Float)\n";
          }
          Value* is_ref = check_is_reference(obj);
          create_conditional_branch(use_cache, negative, is_ref);
          set_block(use_cache);

          Value* is_type = check_type_bits(obj, FloatType);
          create_conditional_branch(positive, negative, is_type);
        } else if(class_id == llvm_state()->symbol_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against Symbol)\n";
          }
          Value* is_symbol = check_is_symbol(obj);
          create_conditional_branch(positive, negative, is_symbol);
        } else if(class_id == llvm_state()->string_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against String)\n";
          }
          Value* is_ref = check_is_reference(obj);
          create_conditional_branch(use_cache, negative, is_ref);
          set_block(use_cache);

          Value* is_type = check_type_bits(obj, StringType);
          create_conditional_branch(positive, negative, is_type);
        } else if(class_id == llvm_state()->regexp_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against Regexp)\n";
          }
          Value* is_ref = check_is_reference(obj);
          create_conditional_branch(use_cache, negative, is_ref);
          set_block(use_cache);

          Value* is_type = check_type_bits(obj, RegexpType);
          create_conditional_branch(positive, negative, is_type);
        } else if(class_id == llvm_state()->encoding_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against Encoding)\n";
          }
          Value* is_ref = check_is_reference(obj);
          create_conditional_branch(use_cache, negative, is_ref);
          set_block(use_cache);

          Value* is_type = check_type_bits(obj, EncodingType);
          create_conditional_branch(positive, negative, is_type);
        } else if(class_id == llvm_state()->module_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against Module)\n";
          }
          Value* is_ref = check_is_reference(obj);
          create_conditional_branch(use_cache, negative, is_ref);
          set_block(use_cache);

          Value* is_type = check_type_bits(obj, ModuleType);
          // Use call here since Class has a different VM type
          // but still is a Module.
          create_conditional_branch(positive, use_call, is_type);
        } else if(class_id == llvm_state()->class_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against Class)\n";
          }
          Value* is_ref = check_is_reference(obj);
          create_conditional_branch(use_cache, negative, is_ref);
          set_block(use_cache);

          Value* is_type = check_type_bits(obj, ClassType);
          create_conditional_branch(positive, use_call, is_type);
        } else if(class_id == llvm_state()->true_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against TrueClass)\n";
          }
          Value* is_true = create_equal(obj, constant(cTrue), "is_true");
          create_conditional_branch(positive, negative, is_true);
        } else if(class_id == llvm_state()->false_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against FalseClass)\n";
          }
          Value* is_false = create_equal(obj, constant(cFalse), "is_false");
          create_conditional_branch(positive, negative, is_false);
        } else if(class_id == llvm_state()->nil_class_id()) {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(against NilClass)\n";
          }
          Value* is_nil = create_equal(obj, constant(cNil), "is_nil");
          create_conditional_branch(positive, negative, is_nil);
        } else {
          if(llvm_state()->config().jit_inline_debug) {
            ctx_->log() << "(regular Ruby class)\n";
          }
          check_direct_class(obj, check_klass, positive, use_call, true);
        }
      }

      if(!class_id) {
        if(llvm_state()->config().jit_inline_debug) {
          ctx_->inline_log("inlining") << "no cache for kind_of fast path\n";
        }
        check_direct_class(obj, check_klass, positive, use_call, true);
      }

      set_block(positive);
      immediate_positive_block = current_block();
      create_branch(cont);

      set_block(negative);
      immediate_negative_block = current_block();
      create_branch(cont);

      set_block(use_call);

      Signature sig(ctx_, ctx_->ptr_type("Object"));
      sig << "State";
      sig << "Object";
      sig << "Object";

      Value* call_args[] = { state_, obj, check_klass };

      CallInst* val = sig.call("rbx_kind_of", call_args, 3, "kind_of", b());
      val->setOnlyReadsMemory();
      val->setDoesNotThrow();

      BasicBlock* ret_block = current_block();
      create_branch(cont);
      set_block(cont);

      PHINode* phi = b().CreatePHI(ObjType, 3, "constant");
      phi->addIncoming(constant(cTrue), immediate_positive_block);
      phi->addIncoming(constant(cFalse), immediate_negative_block);
      phi->addIncoming(val, ret_block);
      return phi;
    }

    void verify_guard(Value* cmp, BasicBlock* failure) {
      BasicBlock* cont = new_block("guarded_body");
      create_conditional_branch(cont, failure, cmp);

      set_block(cont);

      failure->moveAfter(cont);
    }

    type::KnownType check_class(Value* obj, Class* klass, uint32_t class_id, uint32_t serial_id,
                                BasicBlock* class_id_failure, BasicBlock* serial_id_failure) {

      object_type type = (object_type)klass->instance_type()->to_native();

      switch(type) {
      case rubinius::Symbol::type:
        if(ctx_->llvm_state()->type_optz()) {
          type::KnownType kt = type::KnownType::extract(ctx_, obj);

          if(kt.symbol_p()) {
            ctx_->info("eliding guard: detected symbol");
            return kt;
          } else {
            verify_guard(check_is_symbol(obj), class_id_failure);
          }
        } else {
          verify_guard(check_is_symbol(obj), class_id_failure);
        }

        return type::KnownType::symbol();
      case rubinius::Fixnum::type:
        {
          if(ctx_->llvm_state()->type_optz()) {
            type::KnownType kt = type::KnownType::extract(ctx_, obj);

            if(kt.static_fixnum_p()) {
              ctx_->info("eliding guard: detected static fixnum");
              return kt;
            } else {
              verify_guard(check_is_fixnum(obj), class_id_failure);
            }
          } else {
            verify_guard(check_is_fixnum(obj), class_id_failure);
          }
        }
        return type::KnownType::fixnum();
      case NilType:
        verify_guard(check_is_immediate(obj, cNil), class_id_failure);
        return type::KnownType::nil();
      case TrueType:
        verify_guard(check_is_immediate(obj, cTrue), class_id_failure);
        return type::KnownType::true_();
      case FalseType:
        verify_guard(check_is_immediate(obj, cFalse), class_id_failure);
        return type::KnownType::false_();
      default:
        {
          type::KnownType kt = type::KnownType::extract(ctx_, obj);

          if(kt.type_p()) {
            if(ctx_->llvm_state()->config().jit_inline_debug) {
              ctx_->info_log("eliding guard") << "Type object used statically\n";
            }

            return kt;

          } else if(kt.class_id() == class_id) {
            if(ctx_->llvm_state()->config().jit_inline_debug) {
              ctx_->info_log("eliding redundant guard")
                << "class " << ctx_->llvm_state()->symbol_debug_str(klass->module_name())
                << " (" << class_id << ")\n";
            }

            return kt;
          }

          check_reference_class(obj, class_id, serial_id, class_id_failure, serial_id_failure);
          if(SingletonClass* singleton = try_as<SingletonClass>(klass)) {
            if(Class* attached_class = try_as<Class>(singleton->attached_instance())) {
              return type::KnownType::class_object(attached_class->class_id());
            } else {
              return type::KnownType::singleton_instance(class_id);
            }
          } else {
            return type::KnownType::instance(class_id);
          }
        }
      }
    }

    type::KnownType check_class(Value* obj, Class* klass,
                                BasicBlock* failure) {

      object_type type = (object_type)klass->instance_type()->to_native();

      switch(type) {
      case rubinius::Symbol::type:
        if(ctx_->llvm_state()->type_optz()) {
          type::KnownType kt = type::KnownType::extract(ctx_, obj);

          if(kt.symbol_p()) {
            ctx_->info("eliding guard: detected symbol");
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
          if(ctx_->llvm_state()->type_optz()) {
            type::KnownType kt = type::KnownType::extract(ctx_, obj);

            if(kt.fixnum_p()) {
              ctx_->info("eliding guard: detected fixnum");
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
          type::KnownType kt = type::KnownType::extract(ctx_, obj);

          if(kt.type_p()) {
            if(ctx_->llvm_state()->config().jit_inline_debug) {
              ctx_->info_log("eliding guard") << "Type object used statically\n";
            }

            return kt;

          } else if(kt.class_id() == klass->class_id()) {
            if(ctx_->llvm_state()->config().jit_inline_debug) {
              ctx_->info_log("eliding redundant guard")
                << "class " << ctx_->llvm_state()->symbol_debug_str(klass->module_name())
                << " (" << klass->class_id() << ")\n";
            }

            return kt;
          }

          check_reference_class(obj, klass, failure);
          if(kind_of<SingletonClass>(klass)) {
            return type::KnownType::singleton_instance(klass->class_id());
          } else {
            return type::KnownType::instance(klass->class_id());
          }
        }
      }
    }

    void check_reference_class(Value* obj, Class* klass, BasicBlock* failure) {
      Value* is_ref = check_is_reference(obj);
      BasicBlock* cont = new_block("check_class_id");
      BasicBlock* body = new_block("correct_class");

      create_conditional_branch(cont, failure, is_ref);

      set_block(cont);

      Value* target = reference_class(obj);
      Value* match = create_equal(get_class_id(target), cint(klass->class_id()), "check_class_id");

      create_conditional_branch(body, failure, match);

      set_block(body);

      failure->moveAfter(body);
    }

    void check_reference_class(Value* obj, uint32_t class_id, uint32_t serial_id,
                               BasicBlock* class_id_failure, BasicBlock* serial_id_failure) {
      Value* is_ref = check_is_reference(obj);
      BasicBlock* cont = new_block("check_class_id");
      BasicBlock* body = new_block("correct_class");
      BasicBlock* serial = new_block("correct_serial");

      create_conditional_branch(cont, class_id_failure, is_ref);

      set_block(cont);

      Value* klass = reference_class(obj);
      Value* class_match = create_equal(get_class_id(klass), cint(class_id), "check_class_id");

      create_conditional_branch(body, class_id_failure, class_match);

      set_block(body);
      class_id_failure->moveAfter(body);

      Value* serial_match = create_equal(get_serial_id(klass), cint(serial_id), "check_serial_id");

      create_conditional_branch(serial, serial_id_failure, serial_match);

      set_block(serial);

      serial_id_failure->moveAfter(serial);
    }

    // BasicBlock management
    //
    BasicBlock* current_block() {
      return b().GetInsertBlock();
    }

    BasicBlock* new_block(const char* name = "continue") {
      return BasicBlock::Create(ctx_->llvm_context(), name, function_);
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

      if(type::KnownType::has_hint(ctx_, val)) {
        set_known_type(type::KnownType::extract(ctx_, val));
      }
    }

    void stack_push(Value* val, type::KnownType kt) {
      kt.associate(ctx_, val);

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
      kt.associate(ctx_, I);

      return I;
    }

    llvm::Value* stack_top() {
      return stack_back(0);
    }

    void stack_set_top(Value* val) {
      b().CreateStore(val, stack_ptr());

      clear_hint();

      if(type::KnownType::has_hint(ctx_, val)) {
        set_known_type(type::KnownType::extract(ctx_, val));
      }
    }

    llvm::Value* stack_pop() {
      Value* val = stack_back(0);

      stack_ptr_adjust(-1);
      return val;
    }

    // Scope maintenance
    void flush_scope_to_heap(Value* vars) {
      Value* pos = b().CreateConstGEP2_32(vars, 0, offset::StackVariables::on_heap,
                                     "on_heap_pos");

      Value* on_heap = b().CreateLoad(pos, "on_heap");

      Value* null = ConstantExpr::getNullValue(on_heap->getType());
      Value* cmp = create_not_equal(on_heap, null, "null_check");

      BasicBlock* do_flush = new_block("do_flush");
      BasicBlock* cont = new_block("continue");

      create_conditional_branch(do_flush, cont, cmp);

      set_block(do_flush);

      Signature sig(ctx_, "Object");
      sig << "State";
      sig << "StackVariables";

      Value* call_args[] = { state_, vars };

      sig.call("rbx_flush_scope", call_args, 2, "", b());

      create_branch(cont);

      set_block(cont);
    }

    // Constant creation
    //
    Value* constant(Object* obj) {
      return b().CreateIntToPtr(
          ConstantInt::get(ctx_->IntPtrTy, (intptr_t)obj),
          ObjType, "const_obj");
    }

    Value* constant(void* obj, Type* type) {
      return b().CreateIntToPtr(
          ConstantInt::get(ctx_->IntPtrTy, (intptr_t)obj),
          type, "const_of_type");
    }

    Value* ptrtoint(Value* ptr) {
      return b().CreatePtrToInt(ptr, ctx_->IntPtrTy, "ptr2int");
    }

    Value* subtract_pointers(Value* ptra, Value* ptrb) {
      Value* inta = ptrtoint(ptra);
      Value* intb = ptrtoint(ptrb);

      Value* sub = b().CreateSub(inta, intb, "ptr_diff");

      Value* size_of = ConstantInt::get(ctx_->IntPtrTy, sizeof(uintptr_t));

      return b().CreateSDiv(sub, size_of, "ptr_diff_adj");
    }

    // numeric manipulations
    //
    Value* cast_int(Value* obj) {
      return b().CreatePtrToInt(
          obj, NativeIntTy, "cast");
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

      return b().CreateAShr(i, One, "ashr");
    }

    Value* as_obj(Value* val) {
      return b().CreateIntToPtr(val, ObjType, "as_obj");
    }

    Value* check_if_fixnum(Value* val) {
      Value* fix_mask = ConstantInt::get(ctx_->IntPtrTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ctx_->IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(val);
      Value* masked = b().CreateAnd(lint, fix_mask, "masked");

      return b().CreateICmpEQ(masked, fix_tag, "is_fixnum");
    }

    Value* check_if_positive_fixnum(Value* val) {
      /**
       * Add the sign bit to the mask, when and'ed the result
       * should be the default fixnum mask for a positive number
       */
      Value* fix_mask = ConstantInt::get(ctx_->IntPtrTy,
                        (1UL << (FIXNUM_WIDTH + 1)) | TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ctx_->IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(val);
      Value* masked = b().CreateAnd(lint, fix_mask, "masked");

      return b().CreateICmpEQ(masked, fix_tag, "is_fixnum");
    }

    Value* check_if_fixnums(Value* val, Value* val2) {
      Value* fix_mask = ConstantInt::get(ctx_->IntPtrTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ctx_->IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(val);
      Value* rint = cast_int(val2);

      Value* anded = b().CreateAnd(lint, rint, "fixnums_anded");
      Value* masked = b().CreateAnd(anded, fix_mask, "masked");

      return b().CreateICmpEQ(masked, fix_tag, "is_fixnum");
    }

    Value* check_if_positive_fixnums(Value* val, Value* val2) {
      /**
       * Add the sign bit to the mask, when and'ed the result
       * should be the default fixnum mask for a positive number
       */
      Value* fix_mask = ConstantInt::get(ctx_->IntPtrTy,
                        (1UL << (FIXNUM_WIDTH + 1)) | TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ctx_->IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(val);
      Value* rint = cast_int(val2);

      /**
       * And both numbers with the mask for positive numbers.
       * Then OR them so if one of them is negative, the result
       * is not equal to the fixnum tag obtained from both arguments
       */
      Value* masked1 = b().CreateAnd(lint, fix_mask, "masked");
      Value* masked2 = b().CreateAnd(rint, fix_mask, "masked");
      Value* anded   = b().CreateAnd(masked1, masked2, "fixnums_anded");
      Value* ored    = b().CreateOr(masked1, masked2, "fixnums_ored");

      Value* and_fix = b().CreateICmpEQ(anded, fix_tag, "is_fixnum_and");
      Value* ored_fix = b().CreateICmpEQ(ored, fix_tag, "is_fixnum_ored");

      return b().CreateAnd(and_fix, ored_fix, "is_fixnum");
    }

    Value* check_if_non_zero_fixnum(Value* val) {
      Value* fix_tag  = ConstantInt::get(ctx_->IntPtrTy, TAG_FIXNUM);

      Value* lint = cast_int(val);
      return b().CreateICmpNE(lint, fix_tag, "is_fixnum");
    }

    Value* check_if_fits_fixnum(Value* val) {
      Value* fixnum_max = ConstantInt::get(ctx_->IntPtrTy, FIXNUM_MAX);
      Value* fixnum_min = ConstantInt::get(ctx_->IntPtrTy, FIXNUM_MIN);
      Value* val_int = cast_int(val);

      Value* less = b().CreateICmpSLE(val_int, fixnum_max);
      Value* more = b().CreateICmpSGE(val_int, fixnum_min);

      return b().CreateAnd(less, more, "fits_fixnum");
    }

    Value* promote_to_bignum(Value* arg) {
      std::vector<Type*> types;

      types.push_back(StateTy);
      types.push_back(NativeIntTy);

      FunctionType* ft = FunctionType::get(ObjType, types, false);
      Function* func = cast<Function>(
          module_->getOrInsertFunction("rbx_create_bignum", ft));

      Value* call_args[] = {
        state_,
        arg
      };

      CallInst* result = b().CreateCall(func, call_args, "big_value");
      result->setDoesNotThrow();
      return result;
    }

    // Tuple access
    Value* get_tuple_size(Value* tup) {
      Value* idx[] = {
        zero_,
        cint(offset::Tuple::full_size)
      };

      Value* pos = create_gep(tup, idx, 2, "table_size_pos");

      Value* bytes = b().CreateIntCast(
                        create_load(pos, "table_size"),
                        NativeIntTy,
                        true,
                        "to_native_int");

      Value* header = ConstantInt::get(NativeIntTy, Tuple::fields_offset);
      Value* body_bytes = b().CreateSub(bytes, header);

      Value* ptr_size = ConstantInt::get(NativeIntTy, sizeof(Object*));
      return b().CreateSDiv(body_bytes, ptr_size);
    }

    // Tuple access
    Value* get_bytearray_size(Value* bytearray) {
      Value* idx[] = {
        zero_,
        cint(offset::ByteArray::full_size)
      };

      Value* pos = create_gep(bytearray, idx, 2, "bytearray_size_pos");

      Value* bytes = b().CreateIntCast(
                        create_load(pos, "bytearray_size"),
                        NativeIntTy,
                        true,
                        "to_native_int");

      Value* header = ConstantInt::get(NativeIntTy, ByteArray::bytes_offset);
      Value* body_bytes = b().CreateSub(bytes, header);

      return body_bytes;
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

    void set_object_type_slot(Value* obj, int field, int offset, object_type type, Value* val) {
      BasicBlock* not_nil = new_block("not_nil");
      BasicBlock* cont    = new_block("type_verified");
      BasicBlock* failure = new_block("type_unverified");
      BasicBlock* done    = new_block("done");
      BasicBlock* ref     = new_block("is_reference");


      create_conditional_branch(cont, not_nil, check_is_immediate(val, cNil));
      set_block(not_nil);

      switch(type) {
        case Fixnum::type:
        case Integer::type:
          // Optimize here for the common case that the integer is a fixnum
          create_conditional_branch(cont, failure, check_is_fixnum(val));
          break;
        case Symbol::type:
          create_conditional_branch(cont, failure, check_is_symbol(val));
          break;
        case Object::type:
          // Any type here is a valid type, so no need for a type check
          create_branch(cont);
          break;
        default:
          create_conditional_branch(ref, failure, check_is_reference(val));
          set_block(ref);
          create_conditional_branch(cont, failure, check_type_bits(val, type));
          break;
      }

      set_block(cont);
      set_object_slot(obj, offset, val);
      cont = current_block();
      create_branch(done);

      set_block(failure);

      Signature sig(ctx_, ObjType);

      sig << StateTy;
      sig << ObjType;
      sig << ctx_->Int32Ty;
      sig << ObjType;

      Value* call_args[] = {
        state_,
        obj,
        cint(field),
        val
      };

      Value* ret = sig.call("rbx_set_my_field", call_args, 4, "field", b());
      check_for_exception(ret, false);

      failure = current_block();

      create_branch(done);

      set_block(done);

      PHINode* phi = b().CreatePHI(ObjType, 2, "push_ivar");
      phi->addIncoming(val, cont);
      phi->addIncoming(ret, failure);
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

      Value* is_ref = check_is_reference(val);

      BasicBlock* cont = new_block("reference_obj");
      BasicBlock* cont_notscan = new_block("object_not_scanned");

      BasicBlock* run_barrier = new_block("run_barrier");
      BasicBlock* done = new_block("done");

      create_conditional_branch(cont, done, is_ref);

      set_block(cont);

      // Check if obj is scanned already
      Value* object_memory_mark = b().CreateLoad(object_memory_mark_pos_, "object_memory_mark");
      Value* flags = object_flags(obj);

      Value* mark_mask  = ConstantInt::get(ctx_->Int64Ty, (7UL << (OBJECT_FLAGS_MARKED - 2)));
      Value* mark_masked = b().CreateAnd(flags, mark_mask, "mark_mask");
      Value* scan_val   = b().CreateIntCast(
                            b().CreateAdd(object_memory_mark, cint(1), "scan_mark"),
                            ctx_->Int64Ty, false);

      Value* scan_mark = b().CreateShl(scan_val, llvm::ConstantInt::get(ctx_->Int64Ty, OBJECT_FLAGS_MARKED - 2), "lshr");
      Value* is_scanned = b().CreateICmpEQ(mark_masked, scan_mark, "is_scanned");

      create_conditional_branch(run_barrier, cont_notscan, is_scanned);
      set_block(cont_notscan);

      Value* zone_mask  = ConstantInt::get(ctx_->Int64Ty, (3UL << (OBJECT_FLAGS_GC_ZONE - 1)));
      Value* young_mask = ConstantInt::get(ctx_->Int64Ty, YoungObjectZone << (OBJECT_FLAGS_GC_ZONE - 1));

      Value* zone_masked = b().CreateAnd(flags, zone_mask, "zone_mask");
      Value* is_young = b().CreateICmpEQ(zone_masked, young_mask, "is_young");

      create_conditional_branch(done, run_barrier, is_young);
      set_block(run_barrier);

      Signature wb(ctx_, ObjType);
      wb << StateTy;
      wb << ObjType;
      wb << ObjType;

      if(obj->getType() != ObjType) {
        obj = b().CreateBitCast(obj, ObjType, "casted");
      }

      Value* call_args[] = { state_, obj, val };
      wb.call("rbx_write_barrier", call_args, 3, "", b());

      create_branch(done);
      set_block(done);

    }

    void call_debug_spot(int spot) {
      Signature sig(ctx_, ObjType);
      sig << ctx_->Int32Ty;

      Value* call_args[] = { cint(spot) };

      sig.call("rbx_jit_debug_spot", call_args, 1, "", b());
    }

    void call_debug_spot(Value* val) {
      Signature sig(ctx_, ObjType);
      sig << val->getType();

      Value* call_args[] = { val };

      sig.call("rbx_jit_debug_spot", call_args, 1, "", b());
    }

    virtual void check_for_exception(llvm::Value* val, bool pass_top=true) = 0;
  };
}

#endif
