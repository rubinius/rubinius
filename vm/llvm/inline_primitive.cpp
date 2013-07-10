#ifdef ENABLE_LLVM

#include "llvm/inline.hpp"
#include "llvm/jit_context.hpp"

// We use the i64 and i32 versions here. We do the bounds checking
// manually later on, because using i63 / i31 is buggy in LLVM, also
// see http://llvm.org/bugs/show_bug.cgi?id=13991
#ifdef IS_X8664
#define MUL_WITH_OVERFLOW "llvm.smul.with.overflow.i64"
#else
#define MUL_WITH_OVERFLOW "llvm.smul.with.overflow.i32"
#endif

namespace rubinius {

  enum MathOperation {
    cAdd, cSub, cMultiply, cDivide, cMod,
    cEqual, cLessThan, cLessThanEqual, cGreaterThan, cGreaterThanEqual
  };

  struct InlinePrimitive {
    JITOperations& ops;
    Inliner& i;
    CompiledCode* compiled_code;
    Class* klass;
    ClassData data;

    InlinePrimitive(JITOperations& _ops, Inliner& _i, CompiledCode* _code,
                    Class* _klass, ClassData _data)
      : ops(_ops)
      , i(_i)
      , compiled_code(_code)
      , klass(_klass)
      , data(_data)
    {}

    void log(const char* name) {
      if(ops.llvm_state()->config().jit_inline_debug) {
        std::string klass_name = ops.llvm_state()->symbol_debug_str(klass->module_name());
        i.context()->inline_log("inlining")
          << ops.llvm_state()->enclosure_name(compiled_code)
          << "#"
          << ops.llvm_state()->symbol_debug_str(compiled_code->name())
          << " into "
          << ops.llvm_state()->symbol_debug_str(ops.method_name())
          << ". primitive " << name << " (" << klass_name << ")\n";
      }
    }

    void tuple_at() {
      log("tuple_at");
      i.context()->enter_inline();

      Value* rec = i.recv();

      // bool is_tuple = recv->flags & mask;

      Value* cmp = ops.check_is_tuple(rec);

      BasicBlock* is_tuple = ops.new_block("is_tuple");
      BasicBlock* access =   ops.new_block("tuple_at");
      BasicBlock* is_other = i.class_id_failure();

      ops.create_conditional_branch(is_tuple, is_other, cmp);

      ops.set_block(is_tuple);

      Value* index_val = i.arg(0);

      Value* fix_cmp = ops.check_if_positive_fixnum(index_val);
      // Check that index is not over the end of the Tuple

      Value* tup = ops.upcast(rec, "Tuple");

      Value* index = ops.fixnum_strip(index_val);

      Value* full_size = ops.get_tuple_size(tup);
      Value* size_cmp = ops.create_less_than(index, full_size, "is_in_bounds");

      // Combine fix_cmp and size_cmp to validate entry into access code
      Value* access_cmp = ops.create_and(fix_cmp, size_cmp, "access_cmp");
      ops.create_conditional_branch(access, is_other, access_cmp);

      ops.set_block(access);

      Value* idx[] = {
        ConstantInt::get(ops.context()->Int32Ty, 0),
        ConstantInt::get(ops.context()->Int32Ty, offset::Tuple::field),
        index
      };

      Value* gep = ops.create_gep(tup, idx, 3, "field_pos");

      i.exception_safe();
      i.set_result(ops.create_load(gep, "tuple_at"));

      i.context()->leave_inline();
    }

    void tuple_put() {
      log("tuple_put");
      i.context()->enter_inline();

      Value* rec = i.recv();

      Value* cmp = ops.check_is_tuple(rec);

      BasicBlock* is_tuple = ops.new_block("is_tuple");
      BasicBlock* access =   ops.new_block("tuple_put");
      BasicBlock* is_other = i.class_id_failure();

      ops.create_conditional_branch(is_tuple, is_other, cmp);

      ops.set_block(is_tuple);

      Value* index_val = i.arg(0);
      Value* fix_cmp = ops.check_if_positive_fixnum(index_val);

      Value* tup = ops.upcast(rec, "Tuple");

      Value* index = ops.fixnum_strip(index_val);
      Value* full_size = ops.get_tuple_size(tup);
      Value* size_cmp = ops.create_less_than(index, full_size, "is_in_bounds");

      // Combine fix_cmp and size_cmp to validate entry into access code
      Value* access_cmp = ops.create_and(fix_cmp, size_cmp, "access_cmp");
      ops.create_conditional_branch(access, is_other, access_cmp);

      ops.set_block(access);

      Value* value = i.arg(1);

      Value* idx[] = {
        ConstantInt::get(ops.context()->Int32Ty, 0),
        ConstantInt::get(ops.context()->Int32Ty, offset::Tuple::field),
        index
      };

      Value* gep = ops.create_gep(tup, idx, 3, "field_pos");

      ops.create_store(value, gep);
      ops.write_barrier(tup, value);

      i.exception_safe();
      i.set_result(value);
      i.context()->leave_inline();
    }

    void tuple_swap() {
      log("tuple_swap");
      i.context()->enter_inline();

      Value* rec = i.recv();

      ops.verify_guard(ops.check_is_tuple(rec), i.class_id_failure());

      Value* left_index = i.arg(0);
      Value* right_index = i.arg(1);

      Value* fix_cmp = ops.check_if_positive_fixnums(left_index, right_index);

      ops.verify_guard(fix_cmp, i.class_id_failure());

      // Check that index is not over the end of the Tuple
      Value* tup = ops.upcast(rec, "Tuple");

      Value* full_size = ops.get_tuple_size(tup);

      Value* lindex = ops.fixnum_strip(left_index);
      Value* lsize_cmp = ops.create_less_than(lindex, full_size, "is_in_bounds");

      Value* rindex = ops.fixnum_strip(right_index);
      Value* rsize_cmp = ops.create_less_than(rindex, full_size, "is_in_bounds");

      // Combine lsize_cmp and rsize_cmp to validate entry into access code
      Value* access_cmp = ops.create_and(lsize_cmp, rsize_cmp, "access_cmp");
      ops.verify_guard(access_cmp, i.class_id_failure());

      Value* lidx[] = {
        ConstantInt::get(ops.context()->Int32Ty, 0),
        ConstantInt::get(ops.context()->Int32Ty, offset::Tuple::field),
        lindex
      };

      Value* lgep = ops.create_gep(tup, lidx, 3, "field_pos");

      Value* ridx[] = {
        ConstantInt::get(ops.context()->Int32Ty, 0),
        ConstantInt::get(ops.context()->Int32Ty, offset::Tuple::field),
        rindex
      };

      Value* rgep = ops.create_gep(tup, ridx, 3, "field_pos");

      Value* left_val = ops.create_load(lgep, "tuple_swap_left");
      Value* right_val = ops.create_load(rgep, "tuple_swap_right");

      ops.b().CreateStore(right_val, lgep);
      ops.b().CreateStore(left_val, rgep);

      i.exception_safe();
      i.set_result(rec);
      i.context()->leave_inline();
    }

    void tuple_fields() {
      log("tuple_fields");
      i.context()->enter_inline();

      Value* rec = i.recv();

      ops.verify_guard(ops.check_is_tuple(rec), i.class_id_failure());

      Value* tup = ops.upcast(rec, "Tuple");

      Value* full_size = ops.get_tuple_size(tup);
      Value* imm_value = ops.fixnum_tag(full_size);

      i.exception_safe();
      i.set_result(imm_value);
      i.context()->leave_inline();
    }

    void bytearray_size() {
      log("bytearray_size");
      i.context()->enter_inline();

      Value* rec = i.recv();

      ops.verify_guard(ops.check_is_bytearray(rec), i.class_id_failure());

      Value* ba = ops.upcast(rec, "ByteArray");

      Value* full_size = ops.get_bytearray_size(ba);
      Value* imm_value = ops.fixnum_tag(full_size);

      i.exception_safe();
      i.set_result(imm_value);
      i.context()->leave_inline();
    }

    void bytearray_get_byte() {
      log("bytearray_get_byte");
      i.context()->enter_inline();

      Value* rec = i.recv();

      // bool is_tuple = recv->flags & mask;

      Value* cmp = ops.check_is_bytearray(rec);

      BasicBlock* is_bytearray = ops.new_block("is_bytearray");
      BasicBlock* access =   ops.new_block("bytearray_get_byte");
      BasicBlock* is_other = i.class_id_failure();

      ops.create_conditional_branch(is_bytearray, is_other, cmp);

      ops.set_block(is_bytearray);

      Value* index_val = i.arg(0);

      Value* fix_cmp = ops.check_if_positive_fixnum(index_val);
      // Check that index is not over the end of the Tuple

      Value* bytearray = ops.upcast(rec, "ByteArray");

      Value* index = ops.fixnum_strip(index_val);

      Value* full_size = ops.get_bytearray_size(bytearray);
      Value* size_cmp = ops.create_less_than(index, full_size, "is_in_bounds");

      // Combine fix_cmp and size_cmp to validate entry into access code
      Value* access_cmp = ops.create_and(fix_cmp, size_cmp, "access_cmp");
      ops.create_conditional_branch(access, is_other, access_cmp);

      ops.set_block(access);

      Value* idx[] = {
        ConstantInt::get(ops.context()->Int32Ty, 0),
        ConstantInt::get(ops.context()->Int32Ty, offset::ByteArray::field),
        index
      };

      Value* gep = ops.create_gep(bytearray, idx, 3, "field_pos");

      Value* result = ops.create_load(gep, "bytearray_get_byte");

      i.set_result(ops.fixnum_tag(result));

      i.exception_safe();
      i.context()->leave_inline();
    }

    void bytearray_set_byte() {
      log("bytearray_set_byte");
      i.context()->enter_inline();

      Value* rec = i.recv();

      // bool is_tuple = recv->flags & mask;

      Value* cmp = ops.check_is_bytearray(rec);

      BasicBlock* is_bytearray  = ops.new_block("is_bytearray");
      BasicBlock* is_arg_fixnum = ops.new_block("is_arg_fixnum");
      BasicBlock* access        = ops.new_block("bytearray_set_byte");

      BasicBlock* is_other = i.class_id_failure();

      ops.create_conditional_branch(is_bytearray, is_other, cmp);

      ops.set_block(is_bytearray);

      Value* index_val = i.arg(0);

      Value* fix_cmp = ops.check_if_positive_fixnum(index_val);
      // Check that index is not over the end of the ByteArray

      Value* bytearray = ops.upcast(rec, "ByteArray");

      Value* index = ops.fixnum_strip(index_val);

      Value* full_size = ops.get_bytearray_size(bytearray);
      Value* size_cmp = ops.create_less_than(index, full_size, "is_in_bounds");

      // Combine fix_cmp and size_cmp to validate entry into access code
      Value* access_cmp = ops.create_and(fix_cmp, size_cmp, "access_cmp");
      ops.create_conditional_branch(is_arg_fixnum, is_other, access_cmp);

      ops.set_block(is_arg_fixnum);

      Value* value = i.arg(1);

      Value* val_cmp = ops.check_if_fixnum(value);
      ops.create_conditional_branch(access, is_other, val_cmp);

      ops.set_block(access);

      Value* idx[] = {
        ConstantInt::get(ops.context()->Int32Ty, 0),
        ConstantInt::get(ops.context()->Int32Ty, offset::Tuple::field),
        index
      };

      GetElementPtrInst* gep = ops.create_gep(bytearray, idx, 3, "field_pos");

      Value* byte = ops.b().CreateIntCast(ops.fixnum_strip(value),
                                          ops.context()->Int8Ty,
                                          true, "cast_byte");

      ops.create_store(byte, gep);

      i.set_result(value);

      i.exception_safe();
      i.context()->leave_inline();
    }


    bool static_fixnum_s_eqq() {
      Value* arg = i.arg(0);

      type::KnownType kt = type::KnownType::extract(ops.context(), arg);
      if(kt.fixnum_p()) {
        log("static_fixnum_s_eqq: true");

        i.exception_safe();
        i.set_result(ops.constant(cTrue));
        return true;
      } else if(!kt.unknown_p()) {
        log("static_fixnum_s_eqq: false");

        i.exception_safe();
        i.set_result(ops.constant(cFalse));
        return true;
      }

      return false;
    }

    void fixnum_s_eqq() {
      log("fixnum_s_eqq");
      i.context()->enter_inline();

      Value* cmp = ops.check_if_fixnum(i.arg(0));

      Value* imm_value = ops.b().CreateSelect(cmp,
          ops.constant(cTrue), ops.constant(cFalse), "is_fixnum");

      i.exception_safe();
      i.set_result(imm_value);
      i.context()->leave_inline();
    }

    bool static_symbol_s_eqq() {
      Value* arg = i.arg(0);

      type::KnownType kt = type::KnownType::extract(ops.context(), arg);
      if(kt.symbol_p()) {
        log("static_symbol_s_eqq: true");

        i.exception_safe();
        i.set_result(ops.constant(cTrue));
        return true;
      } else if(!kt.unknown_p()) {
        log("static_symbol_s_eqq: false");

        i.exception_safe();
        i.set_result(ops.constant(cFalse));
        return true;
      }

      return false;
    }

    void symbol_s_eqq() {
      log("symbol_s_eqq");
      i.context()->enter_inline();

      Value* sym_mask = ConstantInt::get(ops.context()->IntPtrTy, TAG_SYMBOL_MASK);
      Value* sym_tag  = ConstantInt::get(ops.context()->IntPtrTy, TAG_SYMBOL);

      Value* lint = ops.cast_int(i.arg(0));
      Value* masked = ops.b().CreateAnd(lint, sym_mask, "masked");

      Value* cmp = ops.b().CreateICmpEQ(masked, sym_tag, "is_symbol");

      Value* imm_value = ops.b().CreateSelect(cmp,
          ops.constant(cTrue), ops.constant(cFalse), "is_symbol");

      i.exception_safe();
      i.set_result(imm_value);
      i.context()->leave_inline();
    }

    void fixnum_and() {
      log("fixnum_and");
      i.context()->enter_inline();

      Value* lint = ops.cast_int(i.recv());
      Value* rint = ops.cast_int(i.arg(0));

      Value* anded = BinaryOperator::CreateAnd(lint, rint, "fixnums_anded", ops.current_block());

      Value* fix_mask = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM);

      Value* masked = BinaryOperator::CreateAnd(anded, fix_mask, "masked", ops.current_block());

      Value* cmp = ops.create_equal(masked, fix_tag, "is_fixnum");

      BasicBlock* push = ops.new_block("push_bit_and");
      BasicBlock* send = i.class_id_failure();

      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      i.exception_safe();
      i.set_result(ops.as_obj(anded));
      i.context()->leave_inline();
    }

    void fixnum_or() {
      log("fixnum_or");
      i.context()->enter_inline();

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
      BasicBlock* send = i.class_id_failure();

      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      Value* ored = BinaryOperator::CreateOr(lint, rint, "fixnums_ored",
          ops.current_block());


      i.exception_safe();
      i.set_result(ops.as_obj(ored));
      i.context()->leave_inline();
    }

    void fixnum_xor() {
      log("fixnum_xor");
      i.context()->enter_inline();

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
      BasicBlock* send = i.class_id_failure();

      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      Value* xored = BinaryOperator::CreateXor(lint, rint, "fixnums_xored",
          ops.current_block());

      Value* ored  = BinaryOperator::CreateOr(xored, fix_tag, "fixnums_ored",
          ops.current_block());

      i.exception_safe();
      i.set_result(ops.as_obj(ored));
      i.context()->leave_inline();
    }

    void fixnum_neg() {
      log("fixnum_neg");
      i.context()->enter_inline();

      BasicBlock* use_send = i.class_id_failure();
      BasicBlock* inlined = ops.new_block("fixnum_neg");

      Value* self = i.recv();
      Value* cmp = ops.check_if_fixnum(self);
      ops.create_conditional_branch(inlined, use_send, cmp);

      ops.set_block(inlined);
      Value* native = ops.fixnum_strip(self);
      Value* neg = BinaryOperator::CreateSub(
          ops.Zero, native, "to_neg",
          ops.current_block());

      i.exception_safe();
      i.set_result(ops.fixnum_tag(neg));
      i.context()->leave_inline();
    }

    void fixnum_add() {
      log("fixnum_add");
      i.context()->enter_inline();

      Value* recv = i.recv();
      Value* arg = i.arg(0);

      BasicBlock* push = ops.new_block("push_add");
      BasicBlock* tagnow = ops.new_block("tagnow");
      BasicBlock* bignum = ops.new_block("bignum");
      BasicBlock* cont   = ops.new_block("cont");
      BasicBlock* send = i.class_id_failure();

      Value* cmp = ops.check_if_fixnums(recv, arg);
      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      Value* recv_int = ops.fixnum_strip(recv);
      Value* arg_int = ops.fixnum_strip(arg);
      Value* sum = ops.b().CreateAdd(recv_int, arg_int, "fixnum.add");

      Value* check_fits = ops.check_if_fits_fixnum(sum);

      ops.create_conditional_branch(tagnow, bignum, check_fits);

      ops.set_block(bignum);
      Value* big_value = ops.promote_to_bignum(sum);
      ops.create_branch(cont);

      ops.set_block(tagnow);
      Value* imm_value = ops.fixnum_tag(sum);
      ops.create_branch(cont);

      ops.set_block(cont);

      PHINode* res = ops.b().CreatePHI(ops.ObjType, 2, "result");
      res->addIncoming(big_value, bignum);
      res->addIncoming(imm_value, tagnow);

      i.exception_safe();
      i.set_result(res);
      i.use_send_for_failure();
      i.context()->leave_inline();
    }

    void fixnum_sub() {
      log("fixnum_sub");
      i.context()->enter_inline();

      Value* recv = i.recv();
      Value* arg = i.arg(0);

      BasicBlock* push = ops.new_block("push_sub");
      BasicBlock* tagnow = ops.new_block("tagnow");
      BasicBlock* bignum = ops.new_block("bignum");
      BasicBlock* cont   = ops.new_block("cont");
      BasicBlock* send = i.class_id_failure();

      Value* cmp = ops.check_if_fixnums(recv, arg);
      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      Value* recv_int = ops.fixnum_strip(recv);
      Value* arg_int = ops.fixnum_strip(arg);
      Value* sum = ops.b().CreateSub(recv_int, arg_int, "fixnum.sub");

      Value* check_fits = ops.check_if_fits_fixnum(sum);

      ops.create_conditional_branch(tagnow, bignum, check_fits);

      ops.set_block(bignum);
      Value* big_value = ops.promote_to_bignum(sum);
      ops.create_branch(cont);

      ops.set_block(tagnow);
      Value* imm_value = ops.fixnum_tag(sum);
      ops.create_branch(cont);

      ops.set_block(cont);

      PHINode* res = ops.b().CreatePHI(ops.ObjType, 2, "result");
      res->addIncoming(big_value, bignum);
      res->addIncoming(imm_value, tagnow);

      i.exception_safe();
      i.set_result(res);
      i.use_send_for_failure();
      i.context()->leave_inline();
    }

    void fixnum_mul() {
      log("fixnum_mul");
      i.context()->enter_inline();

      Value* recv = i.recv();
      Value* arg = i.arg(0);

      BasicBlock* push = ops.new_block("push_mul");
      BasicBlock* fits = ops.new_block("fits_fixnum");
      BasicBlock* tagnow = ops.new_block("tagnow");
      BasicBlock* send = i.class_id_failure();

      Value* cmp = ops.check_if_fixnums(recv, arg);
      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      std::vector<Type*> types;
      types.push_back(ops.NativeIntTy);
      types.push_back(ops.NativeIntTy);

      std::vector<Type*> struct_types;
      struct_types.push_back(ops.NativeIntTy);
      struct_types.push_back(ops.context()->Int1Ty);

      StructType* st = StructType::get(ops.context()->llvm_context(), struct_types);

      /*
       * We use manual overflow checking here for the case where we do
       * fit in a 64 bit value but overflow a 63 bit type. We can't
       * directly use the LLVM types for this, because of a bug in
       * LLVM:
       *
       * http://llvm.org/bugs/show_bug.cgi?id=13991
       *
       * In future versions this can be simplified if we depend on
       * a version of LLVM that has incorporated this fix.
       */
      FunctionType* ft = FunctionType::get(st, types, false);
      Function* func = cast<Function>(
          ops.context()->module()->getOrInsertFunction(MUL_WITH_OVERFLOW, ft));
      func->setDoesNotThrow();

      Value* recv_int = ops.fixnum_strip(recv);
      Value* arg_int = ops.fixnum_strip(arg);
      Value* call_args[] = { recv_int, arg_int };
      Value* res = ops.b().CreateCall(func, call_args, "mul.overflow");

      Value* sum = ops.b().CreateExtractValue(res, 0, "mul");
      Value* dof = ops.b().CreateExtractValue(res, 1, "did_overflow");

      ops.create_conditional_branch(send, fits, dof);

      ops.set_block(fits);
      Value* check_fits = ops.check_if_fits_fixnum(sum);

      ops.create_conditional_branch(tagnow, send, check_fits);

      ops.set_block(tagnow);
      Value* imm_value = ops.fixnum_tag(sum);

      i.use_send_for_failure();
      i.exception_safe();
      i.set_result(imm_value);
      i.context()->leave_inline();
    }

    void fixnum_div() {
      log("fixnum_div");
      i.context()->enter_inline();

      Value* num = i.recv();
      Value* den = i.arg(0);

      BasicBlock* positive = ops.new_block("positive");
      BasicBlock* divide = ops.new_block("divide");
      BasicBlock* send = i.class_id_failure();

      Value* positive_cmp = ops.check_if_positive_fixnums(num, den);
      ops.create_conditional_branch(positive, send, positive_cmp);

      ops.set_block(positive);

      Value* non_zero = ops.check_if_non_zero_fixnum(den);

      ops.create_conditional_branch(divide, send, non_zero);

      ops.set_block(divide);

      Value* num_int = ops.fixnum_strip(num);
      Value* den_int = ops.fixnum_strip(den);
      Value* div = ops.b().CreateSDiv(num_int, den_int, "fixnum.div");
      Value* result = ops.fixnum_tag(div);

      i.use_send_for_failure();
      i.exception_safe();
      i.set_result(result);
      i.context()->leave_inline();
    }

    void fixnum_mod() {
      log("fixnum_mod");
      i.context()->enter_inline();

      Value* num = i.recv();
      Value* den = i.arg(0);

      BasicBlock* positive = ops.new_block("positive");
      BasicBlock* modulo = ops.new_block("modulo");
      BasicBlock* send = i.class_id_failure();

      Value* positive_cmp = ops.check_if_positive_fixnums(num, den);
      ops.create_conditional_branch(positive, send, positive_cmp);

      ops.set_block(positive);

      Value* non_zero = ops.check_if_non_zero_fixnum(den);

      ops.create_conditional_branch(modulo, send, non_zero);

      ops.set_block(modulo);

      Value* num_int = ops.fixnum_strip(num);
      Value* den_int = ops.fixnum_strip(den);
      Value* rem = ops.b().CreateSRem(num_int, den_int, "fixnum.mod");
      Value* result = ops.fixnum_tag(rem);

      i.use_send_for_failure();
      i.exception_safe();
      i.set_result(result);
      i.context()->leave_inline();
    }

    void fixnum_compare() {
      log("fixnum_compare");
      i.context()->enter_inline();

      Value* lint = ops.cast_int(i.recv());
      Value* rint = ops.cast_int(i.arg(0));

      Value* anded = BinaryOperator::CreateAnd(lint, rint, "fixnums_anded",
          ops.current_block());

      Value* fix_mask = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM_MASK);
      Value* fix_tag  = ConstantInt::get(ops.NativeIntTy, TAG_FIXNUM);

      Value* masked = BinaryOperator::CreateAnd(anded, fix_mask, "masked",
          ops.current_block());

      Value* cmp = ops.create_equal(masked, fix_tag, "is_fixnum");

      BasicBlock* push   = ops.new_block("push_compare");
      BasicBlock* eq     = ops.new_block("equal");
      BasicBlock* neq    = ops.new_block("not_equal");
      BasicBlock* result = ops.new_block("result");
      BasicBlock* send   = i.class_id_failure();

      ops.create_conditional_branch(push, send, cmp);

      ops.set_block(push);

      Value* equal = ops.b().CreateICmpEQ(lint, rint, "fixnum.eq");
      ops.create_conditional_branch(eq, neq, equal);

      ops.set_block(eq);

      Value* eq_result = ops.fixnum_tag(ops.Zero);

      ops.b().CreateBr(result);

      ops.set_block(neq);

      Value* big = ops.b().CreateICmpSLT(lint, rint, "fixnum.lt");

      Value* neq_result = ops.b().CreateSelect(big,
                               ops.fixnum_tag(ops.NegOne),
                               ops.fixnum_tag(ops.One),
                               "select_bool");

      ops.b().CreateBr(result);

      ops.set_block(result);

      PHINode* res = ops.b().CreatePHI(ops.ObjType, 2, "result");
      res->addIncoming(eq_result, eq);
      res->addIncoming(neq_result, neq);

      i.use_send_for_failure();
      i.exception_safe();
      i.set_result(ops.as_obj(res));
      i.context()->leave_inline();

    }

    void fixnum_compare_operation(MathOperation op) {
      log("fixnum_compare_operation");
      i.context()->enter_inline();

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
      BasicBlock* send = i.class_id_failure();

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
          ops.constant(cTrue),
          ops.constant(cFalse));

      i.use_send_for_failure();
      i.exception_safe();
      i.set_result(ops.as_obj(le));
      i.context()->leave_inline();
    }

    void fixnum_left_shift() {
      log("fixnum_left_shift");
      i.context()->enter_inline();

      Value* recv = i.recv();
      Value* shift = i.arg(0);

      BasicBlock* positive = ops.new_block("positive");
      BasicBlock* left_shift = ops.new_block("left_shift");
      BasicBlock* match = ops.new_block("match");
      BasicBlock* tagnow = ops.new_block("tagnow");
      BasicBlock* bignum = ops.new_block("bignum");
      BasicBlock* cont = ops.new_block("cont");
      BasicBlock* send = i.class_id_failure();

      Value* positive_cmp = ops.check_if_positive_fixnums(recv, shift);
      ops.create_conditional_branch(positive, send, positive_cmp);

      ops.set_block(positive);

      Value* recv_int = ops.fixnum_strip(recv);
      Value* shift_int = ops.fixnum_strip(shift);

      Value* small_shift = ops.b().CreateICmpSLT(shift_int, ops.clong(FIXNUM_WIDTH), "small_shift");
      ops.create_conditional_branch(left_shift, send, small_shift);

      ops.set_block(left_shift);

      Value* answer = ops.b().CreateShl(recv_int, shift_int, "shl");
      Value* check  = ops.b().CreateAShr(answer, shift_int, "shr");

      Value* same = ops.b().CreateICmpEQ(recv_int, check, "same");
      ops.create_conditional_branch(match, send, same);

      ops.set_block(match);

      Value* check_fits = ops.check_if_fits_fixnum(answer);

      ops.create_conditional_branch(tagnow, bignum, check_fits);

      ops.set_block(bignum);
      Value* big_value = ops.promote_to_bignum(answer);
      ops.create_branch(cont);

      ops.set_block(tagnow);
      Value* imm_value = ops.fixnum_tag(answer);
      ops.create_branch(cont);

      ops.set_block(cont);

      PHINode* res = ops.b().CreatePHI(ops.ObjType, 2, "result");
      res->addIncoming(big_value, bignum);
      res->addIncoming(imm_value, tagnow);


      i.use_send_for_failure();
      i.exception_safe();
      i.set_result(res);
      i.context()->leave_inline();
    }

    void fixnum_right_shift() {
      log("fixnum_right_shift");
      i.context()->enter_inline();

      Value* recv = i.recv();
      Value* shift = i.arg(0);

      BasicBlock* positive = ops.new_block("positive");
      BasicBlock* right_shift = ops.new_block("right_shift");
      BasicBlock* zero = ops.new_block("zero");
      BasicBlock* cont = ops.new_block("cont");
      BasicBlock* send = i.class_id_failure();

      Value* positive_cmp = ops.check_if_positive_fixnums(recv, shift);
      ops.create_conditional_branch(positive, send, positive_cmp);

      ops.set_block(positive);

      Value* recv_int = ops.fixnum_strip(recv);
      Value* shift_int = ops.fixnum_strip(shift);

      Value* small_shift = ops.b().CreateICmpSLT(shift_int, ops.clong(FIXNUM_WIDTH), "small_shift");
      ops.create_conditional_branch(right_shift, zero, small_shift);

      ops.set_block(right_shift);

      Value* answer  = ops.fixnum_tag(ops.b().CreateAShr(recv_int, shift_int, "shr"));
      ops.create_branch(cont);

      ops.set_block(zero);
      Value* fix_zero = ops.constant(Fixnum::from(0));
      ops.create_branch(cont);

      ops.set_block(cont);
      PHINode* res = ops.b().CreatePHI(ops.ObjType, 2, "result");
      res->addIncoming(answer, right_shift);
      res->addIncoming(fix_zero, zero);

      i.use_send_for_failure();
      i.exception_safe();
      i.set_result(res);
      i.context()->leave_inline();
    }

    void float_op(MathOperation op) {
      log("float_op");
      i.context()->enter_inline();

      i.use_send_for_failure();

      i.check_recv(klass, data);

      Value* arg = i.arg(0);

      BasicBlock* not_float = ops.new_block();

      ops.check_class(arg, klass, not_float);

      // Float#*(Float)
      Value* farg  = ops.b().CreateBitCast(arg, ops.context()->ptr_type("Float"),
          "arg_float");

      Value* unboxed_rhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(farg,  0, offset::Float::val, "arg.value_pos"), "farg");

      BasicBlock* perform = ops.new_block();

      BasicBlock* unbox_block = ops.current_block();

      ops.b().CreateBr(perform);

      // Float#*(Fixnum)
      ops.set_block(not_float);

      ops.verify_guard(ops.check_if_fixnum(arg), i.class_id_failure());

      Value* fix_rhs = ops.b().CreateSIToFP(
          ops.fixnum_strip(arg), unboxed_rhs->getType());

      BasicBlock* convert_block = ops.current_block();

      ops.b().CreateBr(perform);

      // perform operation

      perform->moveAfter(convert_block);
      ops.set_block(perform);

      PHINode* rhs = ops.b().CreatePHI(fix_rhs->getType(), 2, "rhs");
      rhs->addIncoming(unboxed_rhs, unbox_block);
      rhs->addIncoming(fix_rhs, convert_block);

      Value* fself = ops.b().CreateBitCast(i.recv(), ops.context()->ptr_type("Float"),
          "self_float");

      Value* lhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(fself, 0, offset::Float::val, "self.value_pos"), "fself");

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

      Signature sig(ops.context(), ops.context()->ptr_type("Float"));
      sig << "State";

      Function* func = sig.function("rbx_float_allocate");
      func->setDoesNotCapture(1);
      func->setDoesNotAlias(0); // return value
      func->setDoesNotThrow();

      Value* call_args[] = { ops.state() };
      CallInst* res = sig.call("rbx_float_allocate", call_args, 1, "result", ops.b());
      res->setDoesNotThrow();

      type::KnownType kt = type::KnownType::instance(ops.llvm_state()->float_class_id());
      kt.associate(ops.context(), res);

      ops.b().CreateStore(
          performed,
          ops.b().CreateConstGEP2_32(res, 0, offset::Float::val));

      i.exception_safe();
      i.set_result(ops.b().CreateBitCast(res, ops.ObjType));
      i.context()->leave_inline();
    }

    void float_compare() {
      log("float_compare");
      i.context()->enter_inline();

      i.use_send_for_failure();

      i.check_recv(klass, data);

      // Support compare against Floats and Fixnums inline
      BasicBlock* do_compare = ops.new_block("float_compare_operation");
      BasicBlock* check_fix =  ops.new_block("check_fixnum");

      Value* arg = i.arg(0);
      ops.check_class(arg, klass, check_fix);

      Value* farg =  ops.b().CreateBitCast(arg, ops.context()->ptr_type("Float"),
          "arg_float");

      Value* unboxed_rhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(farg,  0, offset::Float::val, "arg.value_pos"), "farg");

      BasicBlock* unboxed_block = ops.current_block();

      ops.b().CreateBr(do_compare);

      ops.set_block(check_fix);
      ops.verify_guard(ops.check_is_fixnum(arg), i.class_id_failure());
      Value* converted_rhs = ops.b().CreateUIToFP(
          ops.fixnum_strip(arg), unboxed_rhs->getType());

      BasicBlock* converted_block = ops.current_block();

      ops.b().CreateBr(do_compare);

      ops.set_block(do_compare);

      do_compare->moveAfter(converted_block);

      PHINode* rhs = ops.b().CreatePHI(converted_rhs->getType(), 2, "float_rhs");
      rhs->addIncoming(unboxed_rhs, unboxed_block);
      rhs->addIncoming(converted_rhs, converted_block);

      Value* fself = ops.b().CreateBitCast(i.recv(), ops.context()->ptr_type("Float"),
          "self_float");
      Value* lhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(fself, 0, offset::Float::val, "self.value_pos"), "fself");

      BasicBlock* eq     = ops.new_block("equal");
      BasicBlock* neq    = ops.new_block("not_equal");
      BasicBlock* result = ops.new_block("result");

      Value* equal = ops.b().CreateFCmpUEQ(lhs, rhs, "float.eq");
      ops.create_conditional_branch(eq, neq, equal);

      ops.set_block(eq);

      Value* eq_result = ops.fixnum_tag(ops.Zero);

      ops.b().CreateBr(result);

      ops.set_block(neq);

      Value* big = ops.b().CreateFCmpULT(lhs, rhs, "fixnum.lt");

      Value* neq_result = ops.b().CreateSelect(big,
                               ops.fixnum_tag(ops.NegOne),
                               ops.fixnum_tag(ops.One),
                               "select_bool");

      ops.b().CreateBr(result);

      ops.set_block(result);

      PHINode* res = ops.b().CreatePHI(ops.ObjType, 2, "result");
      res->addIncoming(eq_result, eq);
      res->addIncoming(neq_result, neq);

      i.exception_safe();
      i.set_result(res);
      i.context()->leave_inline();
    }

    void float_compare_operation(MathOperation op) {
      log("float_compare_operation");
      i.context()->enter_inline();

      i.use_send_for_failure();

      i.check_recv(klass, data);

      // Support compare against Floats and Fixnums inline
      BasicBlock* do_compare = ops.new_block("float_compare_operation");
      BasicBlock* check_fix =  ops.new_block("check_fixnum");

      Value* arg = i.arg(0);
      ops.check_class(arg, klass, check_fix);

      Value* farg =  ops.b().CreateBitCast(arg, ops.context()->ptr_type("Float"),
          "arg_float");

      Value* unboxed_rhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(farg,  0, offset::Float::val, "arg.value_pos"), "farg");

      BasicBlock* unboxed_block = ops.current_block();

      ops.b().CreateBr(do_compare);

      ops.set_block(check_fix);
      ops.verify_guard(ops.check_is_fixnum(arg), i.class_id_failure());
      Value* converted_rhs = ops.b().CreateUIToFP(
          ops.fixnum_strip(arg), unboxed_rhs->getType());

      BasicBlock* converted_block = ops.current_block();

      ops.b().CreateBr(do_compare);

      ops.set_block(do_compare);

      do_compare->moveAfter(converted_block);

      PHINode* rhs = ops.b().CreatePHI(converted_rhs->getType(), 2, "float_rhs");
      rhs->addIncoming(unboxed_rhs, unboxed_block);
      rhs->addIncoming(converted_rhs, converted_block);

      Value* fself = ops.b().CreateBitCast(i.recv(), ops.context()->ptr_type("Float"),
          "self_float");
      Value* lhs = ops.b().CreateLoad(
          ops.b().CreateConstGEP2_32(fself, 0, offset::Float::val, "self.value_pos"), "fself");

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
          ops.constant(cTrue), ops.constant(cFalse), "select_bool");

      i.exception_safe();
      i.set_result(imm_value);
      i.context()->leave_inline();
    }

    void object_equal() {
      log("object_equal");
      i.context()->enter_inline();

      i.check_recv(klass, data);

      Value* cmp = ops.create_equal(i.recv(), i.arg(0), "identity_equal");
      Value* imm_value = SelectInst::Create(cmp, ops.constant(cTrue),
          ops.constant(cFalse), "select_bool", ops.current_block());

      i.exception_safe();
      i.set_result(imm_value);
      i.context()->leave_inline();
    }

    void class_allocate() {
      log("class_allocate");
      i.context()->enter_inline();

      i.check_recv(klass, data);

      Value* cls = i.recv();

      Signature sig(ops.context(), "Object");
      sig << "State";
      sig << "CallFrame";
      sig << "Object";

      Function* func = sig.function("rbx_create_instance");
      func->setDoesNotCapture(1);
      func->setDoesNotCapture(2);
      func->setDoesNotCapture(3);
      func->setDoesNotAlias(0);

      Value* call_args[] = { ops.state(), ops.call_frame(), cls };

      CallInst* out = sig.call("rbx_create_instance", call_args, 3,
                               "instance", ops.b());
      // Even though an allocation actually modifies memory etc, it does
      // not have the semantics that it does. Allocation of this object
      // can be removed if this value is never used afterwards and the
      // allocation can be elided in that case.
      out->setDoesNotThrow();

      type::KnownType kt = type::KnownType::extract(ops.context(), cls);
      if(kt.constant_cache_p()) {
        ConstantCache* constant_cache = kt.constant_cache();
        klass = try_as<Class>(constant_cache->value());
        type::KnownType kt = type::KnownType::unknown();
        if(kind_of<SingletonClass>(klass)) {
          kt = type::KnownType::singleton_instance(klass->class_id());
        } else {
          kt = type::KnownType::instance(klass->class_id());
        }
        kt.associate(ops.context(), out);
      }

      i.set_result(out);
      i.context()->leave_inline();
    }

    void vm_object_equal() {
      log("Type.object_equal");

      Value* cmp = ops.create_equal(i.arg(0), i.arg(1), "identity_equal");
      Value* imm_value = SelectInst::Create(cmp, ops.constant(cTrue),
          ops.constant(cFalse), "select_bool", ops.current_block());

      i.exception_safe();
      i.set_result(imm_value);
    }

    void known_string_hash() {
      log("String#hash");
      i.context()->enter_inline();

      Signature sig(ops.context(), ops.context()->ptr_type("Object"));
      sig << "State";
      sig << "Object";

      Value* call_args[] = { ops.state(), i.recv() };

      CallInst* val = sig.call("rbx_string_hash", call_args, 2, "hash", ops.b());
      // Computing the string hash only reads memory. Even though it has the side
      // effect of storing this hash value, we handle that situation fine anyway
      // by computing it when needed.
      val->setOnlyReadsMemory();
      val->setDoesNotThrow();

      i.exception_safe();
      i.set_result(val);
      i.context()->leave_inline();
    }

    void object_kind_of() {
      log("Object#kind_of");
      i.context()->enter_inline();
      i.set_result(ops.check_kind_of(i.recv(), i.arg(0)));
      i.exception_safe();
      i.context()->leave_inline();
    }

    void vm_object_kind_of() {
      log("Type.object_kind_of");
      i.context()->enter_inline();
      i.set_result(ops.check_kind_of(i.arg(0), i.arg(1)));
      i.exception_safe();
      i.context()->leave_inline();
    }

    void object_class() {
      log("Object#class");
      i.context()->enter_inline();

      Signature sig(ops.context(), ops.context()->ptr_type("Object"));
      sig << "State";
      sig << "Object";

      Value* call_args[] = { ops.state(), i.recv() };

      CallInst* val = sig.call("rbx_class_of", call_args, 2, "object_class", ops.b());
      val->setOnlyReadsMemory();
      val->setDoesNotThrow();

      i.exception_safe();
      i.set_result(val);
      i.context()->leave_inline();
    }

    void vm_object_class() {
      log("Type.object_class");
      i.context()->enter_inline();

      Signature sig(ops.context(), ops.context()->ptr_type("Object"));
      sig << "State";
      sig << "Object";

      Value* call_args[] = { ops.state(), i.arg(0) };

      CallInst* val = sig.call("rbx_class_of", call_args, 2, "object_class", ops.b());
      val->setOnlyReadsMemory();
      val->setDoesNotThrow();

      i.exception_safe();
      i.set_result(val);
      i.context()->leave_inline();
    }

    void regexp_set_last_match() {
      log("Regexp#set_last_match");
      i.context()->enter_inline();

      BasicBlock* reference = ops.new_block("reference");
      BasicBlock* nil = ops.new_block("nil");
      BasicBlock* set = ops.new_block("set");

      Value* matchdata = i.arg(0);

      Value* is_ref = ops.check_is_reference(matchdata);
      ops.create_conditional_branch(reference, nil, is_ref);

      ops.set_block(nil);

      Value* is_nil = ops.create_equal(matchdata, ops.constant(cNil), "is_nil");
      ops.create_conditional_branch(set, i.class_id_failure(), is_nil);

      ops.set_block(reference);

      Value* is_matchdata = ops.check_is_matchdata(matchdata);
      ops.create_conditional_branch(set, i.class_id_failure(), is_matchdata);

      ops.set_block(set);

      Value* call_frame = ops.call_frame();

      Signature sig(ops.context(), ops.ObjType);
      sig << ops.StateTy;
      sig << ops.ObjType;
      sig << "CallFrame";

      Value* call_args[] = {
        ops.state(),
        matchdata,
        call_frame
      };

      CallInst* res = sig.call("rbx_regexp_set_last_match", call_args, 3, "set_last_match", ops.b());
      res->setDoesNotThrow();

      i.set_result(res);
      i.exception_safe();
      i.context()->leave_inline();
    }

    void object_frozen_p() {
      log("Object#frozen?");
      i.context()->enter_inline();
      i.set_result(ops.get_header_value(i.recv(), OBJECT_FLAGS_FROZEN, "rbx_is_frozen"));
      i.exception_safe();
      i.context()->leave_inline();
    }

    void object_tainted_p() {
      log("Object#tainted?");
      i.context()->enter_inline();
      i.set_result(ops.get_header_value(i.recv(), OBJECT_FLAGS_TAINTED, "rbx_is_tainted"));
      i.exception_safe();
      i.context()->leave_inline();
    }

    void object_untrusted_p() {
      log("Object#untrusted?");
      i.context()->enter_inline();
      i.set_result(ops.get_header_value(i.recv(), OBJECT_FLAGS_UNTRUSTED, "rbx_is_untrusted"));
      i.exception_safe();
      i.context()->leave_inline();
    }

    void proc_call(int count) {
      log("Proc#call");
      i.context()->enter_inline();

      Signature sig(ops.context(), ops.ObjType);
      sig << "State";
      sig << "CallFrame";
      sig << "Object";
      sig << ops.context()->Int32Ty;
      sig << ops.ObjArrayTy;

      Value* call_args[] = {
        ops.state(),
        ops.call_frame(),
        i.recv(),
        ops.cint(count),
        ops.stack_objects(count)
      };

      CallInst* res = sig.call("rbx_proc_call", call_args, 5, "result", ops.b());
      res->setDoesNotThrow();

      i.set_result(res);
      i.context()->leave_inline();
    }

    void variable_scope_of_sender() {
      log("VariableScope.of_sender");
      i.context()->enter_inline();

      Signature sig(ops.context(), ops.ObjType);
      sig << "State";
      sig << "CallFrame";

      Value* call_args[] = {
        ops.state(),
        ops.call_frame()
      };

      CallInst* res = sig.call("rbx_variable_scope_of_sender", call_args, 2, "result", ops.b());
      res->setDoesNotThrow();

      i.set_result(res);
      i.exception_safe();
      i.context()->leave_inline();
    }

    void compiledcode_of_sender() {
      log("CompiledCode.of_sender");
      i.context()->enter_inline();

      Signature sig(ops.context(), ops.ObjType);
      sig << "State";
      sig << "CallFrame";

      Value* call_args[] = {
        ops.state(),
        ops.call_frame()
      };

      CallInst* res = sig.call("rbx_compiledcode_of_sender", call_args, 2, "result", ops.b());
      res->setDoesNotThrow();

      i.set_result(res);
      i.exception_safe();
      i.context()->leave_inline();
    }

    void constant_scope_of_sender() {
      log("ConstantScope.of_sender");
      i.context()->enter_inline();

      Signature sig(ops.context(), ops.ObjType);
      sig << "State";
      sig << "CallFrame";

      Value* call_args[] = {
        ops.state(),
        ops.call_frame()
      };

      CallInst* res = sig.call("rbx_constant_scope_of_sender", call_args, 2, "result", ops.b());
      res->setDoesNotThrow();

      i.set_result(res);
      i.exception_safe();
      i.context()->leave_inline();
    }

    void location_of_closest_ruby_method() {
      log("Location.of_closest_ruby_method");
      i.context()->enter_inline();

      Signature sig(ops.context(), ops.ObjType);
      sig << "State";
      sig << "CallFrame";

      Value* call_args[] = {
        ops.state(),
        ops.call_frame()
      };

      CallInst* res = sig.call("rbx_location_of_closest_ruby_method", call_args, 2, "result", ops.b());
      res->setDoesNotThrow();

      i.set_result(res);
      i.exception_safe();
      i.context()->leave_inline();
    }

  };

  bool Inliner::inline_primitive(Class* klass, ClassData data, CompiledCode* code, executor prim) {

    InlinePrimitive ip(ops_, *this, code, klass, data);

    if(prim == Primitives::tuple_at && count_ == 1) {
      ip.tuple_at();
    } else if(prim == Primitives::tuple_put && count_ == 2) {
      ip.tuple_put();
    } else if(prim == Primitives::tuple_fields && count_ == 0) {
      ip.tuple_fields();
    } else if(prim == Primitives::bytearray_size && count_ == 0) {
      ip.bytearray_size();
    } else if(prim == Primitives::bytearray_get_byte && count_ == 1) {
      ip.bytearray_get_byte();
    } else if(prim == Primitives::bytearray_set_byte && count_ == 2) {
      ip.bytearray_set_byte();
    } else if(prim == Primitives::fixnum_and && count_ == 1) {
      ip.fixnum_and();
    } else if(prim == Primitives::fixnum_or && count_ == 1) {
      ip.fixnum_or();
    } else if(prim == Primitives::fixnum_xor && count_ == 1) {
      ip.fixnum_xor();
    } else if(prim == Primitives::fixnum_neg && count_ == 0) {
      ip.fixnum_neg();
    } else if(prim == Primitives::fixnum_compare && count_ == 1) {
      ip.fixnum_compare();
    } else if(prim == Primitives::fixnum_add && count_ == 1) {
      ip.fixnum_add();
    } else if(prim == Primitives::fixnum_sub && count_ == 1) {
      ip.fixnum_sub();
    } else if(prim == Primitives::fixnum_mul && count_ == 1) {
      ip.fixnum_mul();
    } else if(prim == Primitives::fixnum_div && count_ == 1) {
      ip.fixnum_div();
    } else if(prim == Primitives::fixnum_mod && count_ == 1) {
      ip.fixnum_mod();
    } else if(prim == Primitives::fixnum_equal && count_ == 1) {
      ip.fixnum_compare_operation(cEqual);
    } else if(prim == Primitives::fixnum_lt && count_ == 1) {
      ip.fixnum_compare_operation(cLessThan);
    } else if(prim == Primitives::fixnum_le && count_ == 1) {
      ip.fixnum_compare_operation(cLessThanEqual);
    } else if(prim == Primitives::fixnum_gt && count_ == 1) {
      ip.fixnum_compare_operation(cGreaterThan);
    } else if(prim == Primitives::fixnum_ge && count_ == 1) {
      ip.fixnum_compare_operation(cGreaterThanEqual);
    } else if(prim == Primitives::fixnum_left_shift && count_ == 1) {
      ip.fixnum_left_shift();
    } else if(prim == Primitives::fixnum_right_shift && count_ == 1) {
      ip.fixnum_right_shift();
    } else if(prim == Primitives::object_equal && count_ == 1) {
      ip.object_equal();
    } else if(prim == Primitives::vm_object_equal && count_ == 2) {
      ip.vm_object_equal();
    } else if(prim == Primitives::object_kind_of && count_ == 1) {
      ip.object_kind_of();
    } else if(prim == Primitives::vm_object_kind_of && count_ == 2) {
      ip.vm_object_kind_of();
    } else if(prim == Primitives::object_class && count_ == 0) {
      ip.object_class();
    } else if(prim == Primitives::vm_object_class && count_ == 1) {
      ip.vm_object_class();
    } else if(prim == Primitives::regexp_set_last_match && count_ == 1) {
      ip.regexp_set_last_match();
    } else if(prim == Primitives::object_frozen_p && count_ == 0) {
      ip.object_frozen_p();
    } else if(prim == Primitives::object_tainted_p && count_ == 0) {
      ip.object_tainted_p();
    } else if(prim == Primitives::object_untrusted_p && count_ == 0) {
      ip.object_untrusted_p();
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
    } else if(prim == Primitives::float_compare && count_ == 1) {
      ip.float_compare();
    } else if(prim == Primitives::float_equal && count_ == 1) {
      ip.float_compare_operation(cEqual);
    } else if(prim == Primitives::float_lt && count_ == 1) {
      ip.float_compare_operation(cLessThan);
    } else if(prim == Primitives::float_le && count_ == 1) {
      ip.float_compare_operation(cLessThanEqual);
    } else if(prim == Primitives::float_gt && count_ == 1) {
      ip.float_compare_operation(cGreaterThan);
    } else if(prim == Primitives::float_ge && count_ == 1) {
      ip.float_compare_operation(cGreaterThanEqual);
    } else if(prim == Primitives::fixnum_s_eqq && count_ == 1) {
      if(!ip.static_fixnum_s_eqq()) {
        ip.fixnum_s_eqq();
      }
    } else if(prim == Primitives::symbol_s_eqq && count_ == 1) {
      if(!ip.static_symbol_s_eqq()) {
        ip.symbol_s_eqq();
      }
    } else if(prim == Primitives::class_allocate && count_ == 0) {
      ip.class_allocate();
    } else if(prim == Primitives::proc_call) {
      ip.proc_call(count_);
    } else if(prim == Primitives::variable_scope_of_sender && count_ == 0) {
      ip.variable_scope_of_sender();
    } else if(prim == Primitives::compiledcode_of_sender && count_ == 0) {
      ip.compiledcode_of_sender();
    } else if(prim == Primitives::constant_scope_of_sender && count_ == 0) {
      ip.constant_scope_of_sender();
    } else if(prim == Primitives::location_of_closest_ruby_method && count_ == 0) {
      ip.location_of_closest_ruby_method();
    } else {
      if(ops_.llvm_state()->type_optz()) {
        if(prim == Primitives::object_hash && count_ == 0) {
          Value* V = recv();
          type::KnownType kt = type::KnownType::extract(ops_.context(), V);
          if(kt.static_fixnum_p()) {
            exception_safe();
            set_result(ops_.constant(Fixnum::from(kt.value())->fixnum_hash()));

            if(ops_.llvm_state()->config().jit_inline_debug) {
              ops_.context()->inline_log("inlining")
                << "static hash value of " << kt.value() << "\n";
            }

            return true;
          } else if(kt.class_id() == ops_.llvm_state()->string_class_id()) {
            ip.known_string_hash();
            return true;
          }
        }
      }

      JITStubResults stub_res;

      if(Primitives::get_jit_stub(code->prim_index(), stub_res)) {
        if(stub_res.arg_count() == count_) {
          if(ops_.llvm_state()->config().jit_inline_debug) {
            std::string klass_name = ops_.llvm_state()->symbol_debug_str(klass->module_name());
            ops_.context()->inline_log("inlining")
              << ops_.llvm_state()->enclosure_name(code)
              << "#"
              << ops_.llvm_state()->symbol_debug_str(code->name())
              << " into "
              << ops_.llvm_state()->symbol_debug_str(ops_.method_name())
              << ". generic primitive: " << stub_res.name() << " (" << klass_name << ")\n";
          }

          ops_.context()->enter_inline();

          check_recv(klass, data);

          std::vector<Value*> call_args;

          Signature sig(ops_.context(), "Object");
          sig << "State";
          call_args.push_back(ops_.state());

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
          func->setDoesNotCapture(1);
          func->setDoesNotThrow();

          if(stub_res.pass_callframe()) {
            func->setDoesNotCapture(2);
          }

          CallInst* res = sig.call(stub_res.name(), call_args, "prim_value", ops_.b());
          res->setDoesNotThrow();

          // Only doing this when stub_res.can_fail() causes an exception
          // to be thrown when running the ci specs, need to investigate.
          BasicBlock* cont = ops_.new_block("continue");

          Value* as_i = ops_.ptrtoint(res);
          Value* icmp = ops_.b().CreateICmpEQ(as_i,
              ConstantInt::get(ops_.context()->IntPtrTy, reinterpret_cast<intptr_t>(cUndef)));

          use_send_for_failure();

          ops_.b().CreateCondBr(icmp, class_id_failure(), cont);
          ops_.set_block(cont);

          set_result(res);

          ops_.context()->leave_inline();
          return true;
        } else {
          return false;
        }
      } else {
        // Add more primitive inlining!
        if(ops_.llvm_state()->config().jit_inline_debug) {
          ops_.context()->inline_log("NOT inlining")
            << ops_.llvm_state()->enclosure_name(code)
            << "#"
            << ops_.llvm_state()->symbol_debug_str(code->name())
            << " into "
            << ops_.llvm_state()->symbol_debug_str(ops_.method_name())
            << ". No fast stub. primitive: "
            << ops_.llvm_state()->symbol_debug_str(code->primitive())
            << "\n";
        }

        return false;
      }
    }

    return true;
  }

  int Inliner::detect_jit_intrinsic(Class* klass, ClassData data, CompiledCode* code) {
    if(klass->instance_type()->to_native() == rubinius::Tuple::type) {
      if(count_ == 2 && code->name() == ops_.llvm_state()->symbol("swap")) {
        return 1;
      }
    }

    return 0;
  }

  void Inliner::inline_intrinsic(Class* klass, ClassData data, CompiledCode* code, int which) {
    InlinePrimitive ip(ops_, *this, code, klass, data);

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
