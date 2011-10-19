#ifdef ENABLE_LLVM

#include "llvm/inline.hpp"
#include "llvm/jit_context.hpp"

namespace rubinius {

  enum MathOperation {
    cAdd, cSub, cMultiply, cDivide, cMod,
    cEqual, cLessThan, cLessThanEqual, cGreaterThan, cGreaterThanEqual
  };

  struct InlinePrimitive {
    JITOperations& ops;
    Inliner& i;
    CompiledMethod* cm;
    Class* klass;

    InlinePrimitive(JITOperations& _ops, Inliner& _i, CompiledMethod* _cm,
                    Class* _klass)
      : ops(_ops)
      , i(_i)
      , cm(_cm)
      , klass(_klass)
    {}

    void log(const char* name) {
      if(ops.state()->config().jit_inline_debug) {
        i.context().inline_log("inlining")
          << ops.state()->enclosure_name(cm)
          << "#"
          << ops.state()->symbol_debug_str(cm->name())
          << " into "
          << ops.state()->symbol_debug_str(ops.method_name())
          << ". primitive " << name << "\n";
      }
    }

    void tuple_at() {
      log("tuple_at");
      i.context().enter_inline();

      Value* rec = i.recv();

      // bool is_tuple = recv->flags & mask;

      Value* cmp = ops.check_type_bits(rec, rubinius::Tuple::type);

      BasicBlock* is_tuple = ops.new_block("is_tuple");
      BasicBlock* access =   ops.new_block("tuple_at");
      BasicBlock* is_other = i.failure();

      ops.create_conditional_branch(is_tuple, is_other, cmp);

      ops.set_block(is_tuple);

      Value* index_val = i.arg(0);

      Value* fix_cmp = ops.check_if_fixnum(index_val);
      // Check that index is not over the end of the Tuple

      Value* tup = ops.upcast(rec, "Tuple");

      Value* index = ops.fixnum_to_native(index_val);
      Value* full_size = ops.get_tuple_size(tup);
      Value* size_cmp = ops.create_less_than(index, full_size, "is_in_bounds");

      // Combine fix_cmp and size_cmp to validate entry into access code
      Value* access_cmp = ops.create_and(fix_cmp, size_cmp, "access_cmp");
      ops.create_conditional_branch(access, is_other, access_cmp);

      ops.set_block(access);

      Value* idx[] = {
        ConstantInt::get(ops.state()->Int32Ty, 0),
        ConstantInt::get(ops.state()->Int32Ty, offset::tuple_field),
        index
      };

      Value* gep = ops.create_gep(tup, idx, 3, "field_pos");

      i.exception_safe();
      i.set_result(ops.create_load(gep, "tuple_at"));

      i.context().leave_inline();
    }

    void tuple_put() {
      log("tuple_put");
      i.context().enter_inline();

      Value* rec = i.recv();

      Value* cmp = ops.check_type_bits(rec, rubinius::Tuple::type);

      BasicBlock* is_tuple = ops.new_block("is_tuple");
      BasicBlock* access =   ops.new_block("tuple_put");
      BasicBlock* is_other = i.failure();

      ops.create_conditional_branch(is_tuple, is_other, cmp);

      ops.set_block(is_tuple);

      Value* index_val = i.arg(0);
      Value* fix_cmp = ops.check_if_fixnum(index_val);

      Value* tup = ops.upcast(rec, "Tuple");

      Value* index = ops.fixnum_to_native(index_val);
      Value* full_size = ops.get_tuple_size(tup);
      Value* size_cmp = ops.create_less_than(index, full_size, "is_in_bounds");

      // Combine fix_cmp and size_cmp to validate entry into access code
      Value* access_cmp = ops.create_and(fix_cmp, size_cmp, "access_cmp");
      ops.create_conditional_branch(access, is_other, access_cmp);

      ops.set_block(access);

      Value* value = i.arg(1);

      Value* idx[] = {
        ConstantInt::get(ops.state()->Int32Ty, 0),
        ConstantInt::get(ops.state()->Int32Ty, offset::tuple_field),
        index
      };

      Value* gep = ops.create_gep(tup, idx, 3, "field_pos");

      ops.create_store(value, gep);
      ops.write_barrier(tup, value);

      i.exception_safe();
      i.set_result(value);
      i.context().leave_inline();
    }

    void tuple_swap() {
      log("tuple_swap");
      i.context().enter_inline();

      Value* rec = i.recv();

      ops.verify_guard(ops.check_is_tuple(rec), i.failure());

      Value* left_index = i.arg(0);
      Value* right_index = i.arg(1);

      Value* fix_cmp = ops.check_if_fixnums(left_index, right_index);

      ops.verify_guard(fix_cmp, i.failure());

      // Check that index is not over the end of the Tuple
      Value* tup = ops.upcast(rec, "Tuple");

      Value* full_size = ops.get_tuple_size(tup);

      Value* lindex = ops.fixnum_to_native(left_index);
      Value* lsize_cmp = ops.create_less_than(lindex, full_size, "is_in_bounds");

      Value* rindex = ops.fixnum_to_native(right_index);
      Value* rsize_cmp = ops.create_less_than(rindex, full_size, "is_in_bounds");

      // Combine lsize_cmp and rsize_cmp to validate entry into access code
      Value* access_cmp = ops.create_and(lsize_cmp, rsize_cmp, "access_cmp");
      ops.verify_guard(access_cmp, i.failure());

      Value* lidx[] = {
        ConstantInt::get(ops.state()->Int32Ty, 0),
        ConstantInt::get(ops.state()->Int32Ty, offset::tuple_field),
        lindex
      };

      Value* lgep = ops.create_gep(tup, lidx, 3, "field_pos");

      Value* ridx[] = {
        ConstantInt::get(ops.state()->Int32Ty, 0),
        ConstantInt::get(ops.state()->Int32Ty, offset::tuple_field),
        rindex
      };

      Value* rgep = ops.create_gep(tup, ridx, 3, "field_pos");

      Value* left_val = ops.create_load(lgep, "tuple_swap_left");
      Value* right_val = ops.create_load(rgep, "tuple_swap_right");

      ops.b().CreateStore(right_val, lgep);
      ops.b().CreateStore(left_val, rgep);

      i.exception_safe();
      i.set_result(rec);
      i.context().leave_inline();
    }

    bool static_fixnum_s_eqq() {
      Value* arg = i.arg(0);

      type::KnownType kt = type::KnownType::extract(ops.state(), arg);
      if(kt.fixnum_p()) {
        log("static_fixnum_s_eqq: true");

        i.exception_safe();
        i.set_result(ops.constant(Qtrue));
        return true;
      } else if(!kt.unknown_p()) {
        log("static_fixnum_s_eqq: false");

        i.exception_safe();
        i.set_result(ops.constant(Qfalse));
        return true;
      }

      return false;
    }

    void fixnum_s_eqq() {
      log("fixnum_s_eqq");
      i.context().enter_inline();

      Value* cmp = ops.check_if_fixnum(i.arg(0));

      Value* imm_value = ops.b().CreateSelect(cmp,
          ops.constant(Qtrue), ops.constant(Qfalse), "is_fixnum");

      i.exception_safe();
      i.set_result(imm_value);
      i.context().leave_inline();
    }

    bool static_symbol_s_eqq() {
      Value* arg = i.arg(0);

      type::KnownType kt = type::KnownType::extract(ops.state(), arg);
      if(kt.symbol_p()) {
        log("static_symbol_s_eqq: true");

        i.exception_safe();
        i.set_result(ops.constant(Qtrue));
        return true;
      } else if(!kt.unknown_p()) {
        log("static_symbol_s_eqq: false");

        i.exception_safe();
        i.set_result(ops.constant(Qfalse));
        return true;
      }

      return false;
    }

    void symbol_s_eqq() {
      log("symbol_s_eqq");
      i.context().enter_inline();

      Value* sym_mask = ConstantInt::get(ops.state()->IntPtrTy, TAG_SYMBOL_MASK);
      Value* sym_tag  = ConstantInt::get(ops.state()->IntPtrTy, TAG_SYMBOL);

      Value* lint = ops.cast_int(i.arg(0));
      Value* masked = ops.b().CreateAnd(lint, sym_mask, "masked");

      Value* cmp = ops.b().CreateICmpEQ(masked, sym_tag, "is_symbol");

      Value* imm_value = ops.b().CreateSelect(cmp,
          ops.constant(Qtrue), ops.constant(Qfalse), "is_symbol");

      i.exception_safe();
      i.set_result(imm_value);
      i.context().leave_inline();
    }

    void fixnum_and() {
      log("fixnum_and");
      i.context().enter_inline();

      Value* lint = ops.cast_int(i.recv());
      Value* rint = ops.cast_int(i.arg(0));

      Value* anded = BinaryOperator::CreateAnd(lint, rint, "fixnums_anded", ops.current_block());

      Value* fix_mask = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM);

      Value* masked = BinaryOperator::CreateAnd(anded, fix_mask, "masked", ops.current_block());

      Value* cmp = ops.create_equal(masked, fix_tag, "is_fixnum");

      BasicBlock* push = ops.new_block("push_bit_and");
      BasicBlock* send = i.failure();

      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      i.exception_safe();
      i.set_result(ops.as_obj(anded));
      i.context().leave_inline();
    }

    void fixnum_or() {
      log("fixnum_or");
      i.context().enter_inline();

      Value* lint = ops.cast_int(i.recv());
      Value* rint = ops.cast_int(i.arg(0));

      Value* anded = BinaryOperator::CreateAnd(lint, rint, "fixnums_anded",
          ops.current_block());

      Value* fix_mask = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM);

      Value* masked = BinaryOperator::CreateAnd(anded, fix_mask, "masked",
          ops.current_block());

      Value* cmp = ops.create_equal(masked, fix_tag, "is_fixnum");

      BasicBlock* push = ops.new_block("push_bit_or");
      BasicBlock* send = i.failure();

      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      Value* ored = BinaryOperator::CreateOr(lint, rint, "fixnums_ored",
          ops.current_block());


      i.exception_safe();
      i.set_result(ops.as_obj(ored));
      i.context().leave_inline();
    }

    void fixnum_neg() {
      log("fixnum_neg");
      i.context().enter_inline();

      BasicBlock* use_send = i.failure();
      BasicBlock* inlined = ops.new_block("fixnum_neg");

      Value* self = i.recv();
      Value* cmp = ops.check_if_fixnum(self);
      ops.create_conditional_branch(inlined, use_send, cmp);

      ops.set_block(inlined);
      Value* native = ops.fixnum_strip(self);
      Value* neg = BinaryOperator::CreateSub(
          ops.Zero, native, "to_neg",
          ops.current_block());

      Value* more = BinaryOperator::CreateShl(neg, ops.One, "shl", ops.current_block());
      Value* tagged = BinaryOperator::CreateOr(more, ops.One, "or", ops.current_block());

      i.exception_safe();
      i.set_result(ops.as_obj(tagged));
      i.context().leave_inline();
    }

    void fixnum_compare(MathOperation op) {
      log("fixnum_compare");
      i.context().enter_inline();

      Value* lint = ops.cast_int(i.recv());
      Value* rint = ops.cast_int(i.arg(0));

      Value* anded = BinaryOperator::CreateAnd(lint, rint, "fixnums_anded",
          ops.current_block());

      Value* fix_mask = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM);

      Value* masked = BinaryOperator::CreateAnd(anded, fix_mask, "masked",
          ops.current_block());

      Value* cmp = ops.create_equal(masked, fix_tag, "is_fixnum");

      BasicBlock* push = ops.new_block("push_le");
      BasicBlock* send = i.failure();

      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      Value* performed = 0;

      switch(op) {
        case cEqual:
          performed = ops.b().CreateICmpEQ(lint, rint, "fixnum.eq");
          break;
        case cLessThan:
          performed = ops.b().CreateICmpSLT(lint, rint, "fixnum.lt");
          break;
        case cLessThanEqual:
          performed = ops.b().CreateICmpSLE(lint, rint, "fixnum.le");
          break;
        case cGreaterThan:
          performed = ops.b().CreateICmpSGT(lint, rint, "fixnum.gt");
          break;
        case cGreaterThanEqual:
          performed = ops.b().CreateICmpSGE(lint, rint, "fixnum.ge");
          break;
        default:
          rubinius::bug("Unknown fixnum operation in JIT");
      }

      Value* le = ops.b().CreateSelect(
          performed,
          ops.constant(Qtrue),
          ops.constant(Qfalse));

      i.use_send_for_failure();
      i.exception_safe();
      i.set_result(ops.as_obj(le));
      i.context().leave_inline();
    }

    void float_op(MathOperation op) { 
      log("float_op");
      i.context().enter_inline();

      i.use_send_for_failure();

      i.check_recv(klass);

      Value* arg = i.arg(0);

      BasicBlock* not_float = ops.new_block();

      ops.check_class(arg, klass, not_float);

      // Float#*(Float)
      Value* farg  = ops.b().CreateBitCast(arg, ops.state()->ptr_type("Float"),
          "arg_float");

      Value* unboxed_rhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(farg,  0, 1, "arg.value_pos"), "farg");

      BasicBlock* perform = ops.new_block();

      BasicBlock* unbox_block = ops.current_block();

      ops.b().CreateBr(perform);

      // Float#*(Fixnum)
      ops.set_block(not_float);

      ops.verify_guard(ops.check_if_fixnum(arg), i.failure());

      Value* fix_rhs = ops.b().CreateSIToFP(
          ops.fixnum_to_native(arg), unboxed_rhs->getType());

      BasicBlock* convert_block = ops.current_block();

      ops.b().CreateBr(perform);

      // perform operation

      perform->moveAfter(convert_block);
      ops.set_block(perform);

      PHINode* rhs = ops.b().CreatePHI(fix_rhs->getType(), "rhs");
      rhs->addIncoming(unboxed_rhs, unbox_block);
      rhs->addIncoming(fix_rhs, convert_block);

      Value* fself = ops.b().CreateBitCast(i.recv(), ops.state()->ptr_type("Float"),
          "self_float");

      Value* lhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(fself, 0, 1, "self.value_pos"), "fself");

      Value* performed = 0;

      switch(op) {
        case cAdd:
          performed = ops.b().CreateFAdd(lhs, rhs, "float.add");
          break;
        case cSub:
          performed = ops.b().CreateFSub(lhs, rhs, "float.sub");
          break;
        case cMultiply:
          performed = ops.b().CreateFMul(lhs, rhs, "float.mul");
          break;
        case cDivide:
          performed = ops.b().CreateFDiv(lhs, rhs, "float.div");
          break;
        case cMod:
          performed = ops.b().CreateFRem(lhs, rhs, "float.mod");
          break;
        default:
          rubinius::bug("Unknown float operation in JIT");
      }

      Signature sig(ops.state(), ops.state()->ptr_type("Float"));
      sig << "VM";

      Function* func = sig.function("rbx_float_allocate");
      func->setDoesNotAlias(0, true); // return value

      Value* call_args[] = { ops.vm() };
      CallInst* res = sig.call("rbx_float_allocate", call_args, 1, "result", ops.b());

      ops.b().CreateStore(
          performed,
          ops.b().CreateConstGEP2_32(res, 0, 1));

      i.exception_safe();
      i.set_result(ops.b().CreateBitCast(res, ops.ObjType));
      i.context().leave_inline();
    }

    void float_compare(MathOperation op) {
      log("float_compare");
      i.context().enter_inline();

      i.use_send_for_failure();

      i.check_recv(klass);

      // Support compare against Floats and Fixnums inline
      BasicBlock* do_compare = ops.new_block("float_compare");
      BasicBlock* check_fix =  ops.new_block("check_fixnum");

      Value* arg = i.arg(0);
      ops.check_class(arg, klass, check_fix);

      Value* farg =  ops.b().CreateBitCast(arg, ops.state()->ptr_type("Float"),
          "arg_float");

      Value* unboxed_rhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(farg,  0, 1, "arg.value_pos"), "farg");

      BasicBlock* unboxed_block = ops.current_block();

      ops.b().CreateBr(do_compare);

      ops.set_block(check_fix);
      ops.verify_guard(ops.check_is_fixnum(arg), i.failure());
      Value* converted_rhs = ops.b().CreateUIToFP(
          ops.fixnum_to_native(arg), unboxed_rhs->getType());

      BasicBlock* converted_block = ops.current_block();

      ops.b().CreateBr(do_compare);

      ops.set_block(do_compare);

      do_compare->moveAfter(converted_block);

      PHINode* rhs = ops.b().CreatePHI(converted_rhs->getType(), "float_rhs");
      rhs->addIncoming(unboxed_rhs, unboxed_block);
      rhs->addIncoming(converted_rhs, converted_block);

      Value* fself = ops.b().CreateBitCast(i.recv(), ops.state()->ptr_type("Float"),
          "self_float");
      Value* lhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(fself, 0, 1, "self.value_pos"), "fself");

      Value* performed = 0;

      switch(op) {
        case cEqual:
          performed = ops.b().CreateFCmpUEQ(lhs, rhs, "float.eq");
          break;
        case cLessThan:
          performed = ops.b().CreateFCmpULT(lhs, rhs, "float.lt");
          break;
        case cLessThanEqual:
          performed = ops.b().CreateFCmpULE(lhs, rhs, "float.le");
          break;
        case cGreaterThan:
          performed = ops.b().CreateFCmpUGT(lhs, rhs, "float.gt");
          break;
        case cGreaterThanEqual:
          performed = ops.b().CreateFCmpUGE(lhs, rhs, "float.ge");
          break;
        default:
          rubinius::bug("Unknown float operation in JIT");
      }

      Value* imm_value = ops.b().CreateSelect(performed,
          ops.constant(Qtrue), ops.constant(Qfalse), "select_bool");

      i.exception_safe();
      i.set_result(imm_value);
      i.context().leave_inline();
    }

    void object_equal() {
      log("object_equal");
      i.context().enter_inline();

      i.check_recv(klass);

      Value* cmp = ops.create_equal(i.recv(), i.arg(0), "identity_equal");
      Value* imm_value = SelectInst::Create(cmp, ops.constant(Qtrue),
          ops.constant(Qfalse), "select_bool", ops.current_block());

      i.exception_safe();
      i.set_result(imm_value);
      i.context().leave_inline();
    }

    void class_allocate() {
      log("class_allocate");
      i.context().enter_inline();

      i.check_recv(klass);

      Value* V = i.recv();

      Signature sig(ops.state(), "Object");
      sig << "VM";
      sig << "CallFrame";
      sig << "Object";

      Value* call_args[] = { ops.vm(), ops.call_frame(), V };

      Value* out = sig.call("rbx_create_instance", call_args, 3,
                            "instance", ops.b());

      i.set_result(out);
      i.context().leave_inline();
    }

    void type_object_equal() {
      log("type_object_equal");

      Value* cmp = ops.create_equal(i.arg(0), i.arg(1), "identity_equal");
      Value* imm_value = SelectInst::Create(cmp, ops.constant(Qtrue),
          ops.constant(Qfalse), "select_bool", ops.current_block());

      i.exception_safe();
      i.set_result(imm_value);
    }

    void known_string_hash() {
      log("String#hash");
      i.context().enter_inline();

      Signature sig(ops.state(), ops.state()->ptr_type("Object"));
      sig << "VM";
      sig << "Object";

      Value* call_args[] = { ops.vm(), i.recv() };

      Value* val = sig.call("rbx_string_hash", call_args, 2, "hash", ops.b());

      i.exception_safe();
      i.set_result(val);
      i.context().leave_inline();
    }

    /*
       void object_class(Class* klass, JITOperations& ops, Inliner& i) {
       Value* self = i.recv();

       ops.check_class(self, klass, i.failure());

       Signature sig(ops.state(), "Class");
       sig << "VM";
       sig << "Object";

       Value* call_args[] = { ops.vm(), self };

       Value* res = sig.call("rbx_class_of", call_args, 2, "object_class", ops.b());

       i.exception_safe();
       i.set_result(ops.downcast(res));
       }
       */

  };

  bool Inliner::inline_primitive(Class* klass, CompiledMethod* cm, executor prim) {

    InlinePrimitive ip(ops_, *this, cm, klass);

    if(prim == Primitives::tuple_at && count_ == 1) {
      ip.tuple_at();
    } else if(prim == Primitives::tuple_put && count_ == 2) {
      ip.tuple_put();
    } else if(prim == Primitives::fixnum_and && count_ == 1) {
      ip.fixnum_and();
    } else if(prim == Primitives::fixnum_or && count_ == 1) {
      ip.fixnum_or();
    } else if(prim == Primitives::fixnum_neg && count_ == 0) {
      ip.fixnum_neg();
    } else if(prim == Primitives::fixnum_equal && count_ == 1) {
      ip.fixnum_compare(cEqual);
    } else if(prim == Primitives::fixnum_lt && count_ == 1) {
      ip.fixnum_compare(cLessThan);
    } else if(prim == Primitives::fixnum_le && count_ == 1) {
      ip.fixnum_compare(cLessThanEqual);
    } else if(prim == Primitives::fixnum_gt && count_ == 1) {
      ip.fixnum_compare(cGreaterThan);
    } else if(prim == Primitives::fixnum_ge && count_ == 1) {
      ip.fixnum_compare(cGreaterThanEqual);
    } else if(prim == Primitives::object_equal && count_ == 1) {
      ip.object_equal();
    } else if(prim == Primitives::float_add && count_ == 1) {
      ip.float_op(cAdd);
    } else if(prim == Primitives::float_sub && count_ == 1) {
      ip.float_op(cSub);
    } else if(prim == Primitives::float_mul && count_ == 1) {
      ip.float_op(cMultiply);
    } else if(prim == Primitives::float_div && count_ == 1) {
      ip.float_op(cDivide);
    } else if(prim == Primitives::float_mod && count_ == 1) {
      ip.float_op(cMod);
    } else if(prim == Primitives::float_equal && count_ == 1) {
      ip.float_compare(cEqual);
    } else if(prim == Primitives::float_lt && count_ == 1) {
      ip.float_compare(cLessThan);
    } else if(prim == Primitives::float_le && count_ == 1) {
      ip.float_compare(cLessThanEqual);
    } else if(prim == Primitives::float_gt && count_ == 1) {
      ip.float_compare(cGreaterThan);
    } else if(prim == Primitives::float_ge && count_ == 1) {
      ip.float_compare(cGreaterThanEqual);

    } else if(prim == Primitives::fixnum_s_eqq && count_ == 1) {
      if(!ip.static_fixnum_s_eqq()) {
        ip.fixnum_s_eqq();
      }
    } else if(prim == Primitives::symbol_s_eqq && count_ == 1) {
      if(!ip.static_symbol_s_eqq()) {
        ip.symbol_s_eqq();
      }

    } else if(prim == Primitives::vm_object_equal && count_ == 2) {
      ip.type_object_equal();

    } else if(prim == Primitives::class_allocate && count_ == 0) {
      ip.class_allocate();

    } else {
      if(ops_.state()->type_optz()) {
        if(prim == Primitives::object_hash && count_ == 0) {
          Value* V = recv();
          type::KnownType kt = type::KnownType::extract(ops_.state(), V);
          if(kt.static_fixnum_p()) {
            exception_safe();
            set_result(ops_.constant(Fixnum::from(kt.value())->fixnum_hash()));

            if(ops_.state()->config().jit_inline_debug) {
              context_.inline_log("inlining")
                << "static hash value of " << kt.value() << "\n";
            }

            return true;
          } else if(kt.class_id() == ops_.state()->string_class_id()) {
            ip.known_string_hash();
            return true;
          }
        }
      }

      JITStubResults stub_res;

      if(Primitives::get_jit_stub(cm->prim_index(), stub_res)) {
        if(stub_res.arg_count() == count_) {
          if(ops_.state()->config().jit_inline_debug) {
            context_.inline_log("inlining")
              << ops_.state()->enclosure_name(cm)
              << "#"
              << ops_.state()->symbol_debug_str(cm->name())
              << " into "
              << ops_.state()->symbol_debug_str(ops_.method_name())
              << ". generic primitive: " << stub_res.name() << "\n";
          }

          context_.enter_inline();

          check_recv(klass);

          std::vector<Value*> call_args;

          Signature sig(ops_.state(), "Object");
          sig << "VM";
          call_args.push_back(ops_.vm());

          if(stub_res.pass_callframe()) {
            sig << "CallFrame";
            call_args.push_back(ops_.call_frame());
          }

          sig << "Object";
          call_args.push_back(recv());

          for(int i = 0; i < stub_res.arg_count(); i++) {
            sig << "Object";
            call_args.push_back(arg(i));
          }

          Function* func = sig.function(stub_res.name());
          func->setDoesNotCapture(1, true);

          if(stub_res.pass_callframe()) {
            func->setDoesNotCapture(2, true);
          }

          Value* res = sig.call(stub_res.name(), call_args, "prim_value", ops_.b());

          // Only doing this when stub_res.can_fail() causes an exception
          // to be thrown when running the ci specs, need to investigate.
          BasicBlock* cont = ops_.new_block("continue");

          Value* as_i = ops_.ptrtoint(res);
          Value* icmp = ops_.b().CreateICmpEQ(as_i,
              ConstantInt::get(ops_.state()->IntPtrTy, reinterpret_cast<intptr_t>(Qundef)));

          use_send_for_failure();

          ops_.b().CreateCondBr(icmp, failure(), cont);
          ops_.set_block(cont);

          set_result(res);

          context_.leave_inline();
          return true;
        } else {
          return false;
        }
      } else {
        // Add more primitive inlining!
        if(ops_.state()->config().jit_inline_debug) {
          context_.inline_log("NOT inlining")
            << ops_.state()->enclosure_name(cm)
            << "#"
            << ops_.state()->symbol_debug_str(cm->name())
            << " into "
            << ops_.state()->symbol_debug_str(ops_.method_name())
            << ". No fast stub. primitive: "
            << ops_.state()->symbol_debug_str(cm->primitive())
            << "\n";
        }

        return false;
      }
    }

    return true;
  }

  int Inliner::detect_jit_intrinsic(Class* klass, CompiledMethod* cm) {
    if(klass->instance_type()->to_native() == rubinius::Tuple::type) {
      if(count_ == 2 && cm->name() == ops_.state()->symbol("swap")) {
        return 1;
      }
    }

    return 0;
  }

  void Inliner::inline_intrinsic(Class* klass, CompiledMethod* cm, int which) {
    InlinePrimitive ip(ops_, *this, cm, klass);

    switch(which) {
    case 1: // Tuple#swap
      ip.tuple_swap();
      break;
    default:
      rubinius::bug("Unknown inline intrinsic in JIT");
    }
  }
}

#endif
