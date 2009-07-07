#ifdef ENABLE_LLVM

#include "llvm/jit.hpp"
#include "llvm/inline.hpp"
#include "llvm/jit_workhorse.hpp"

namespace rubinius {

  static void call_tuple_at(JITOperations& ops, Inliner& i) {
    Value* rec = i.recv();

    // bool is_tuple = recv->flags & mask;

    Value* cmp = ops.check_type_bits(rec, rubinius::Tuple::type);

    BasicBlock* is_tuple = ops.new_block("is_tuple");
    BasicBlock* access =   ops.new_block("tuple_at");
    BasicBlock* is_other = ops.new_block("is_other");

    ops.create_conditional_branch(is_tuple, is_other, cmp);

    ops.set_block(is_tuple);

    Value* index_val = i.arg(0);

    Value* fix_cmp = ops.check_if_fixnum(index_val);
    // Check that index is not over the end of the Tuple

    Value* tup = ops.upcast(rec, "Tuple");

    Value* index = ops.tag_strip32(index_val);
    Value* full_size = ops.get_tuple_size(tup);
    Value* size_cmp = ops.create_less_than(index, full_size, "is_in_bounds");

    // Combine fix_cmp and size_cmp to validate entry into access code
    Value* access_cmp = ops.create_and(fix_cmp, size_cmp, "access_cmp");
    ops.create_conditional_branch(access, is_other, access_cmp);

    ops.set_block(access);

    ops.stack_remove(2);

    Value* idx[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, offset::tuple_field),
      index
    };

    Value* gep = ops.create_gep(tup, idx, 3, "field_pos");

    ops.stack_push(ops.create_load(gep, "tuple_at"));

    ops.create_branch(i.after());

    ops.set_block(is_other);
  }

  static void call_tuple_put(JITOperations& ops, Inliner& i) {
    Value* rec = i.recv();

    Value* cmp = ops.check_type_bits(rec, rubinius::Tuple::type);

    BasicBlock* is_tuple = ops.new_block("is_tuple");
    BasicBlock* access =   ops.new_block("tuple_put");
    BasicBlock* is_other = ops.new_block("is_other");

    ops.create_conditional_branch(is_tuple, is_other, cmp);

    ops.set_block(is_tuple);

    Value* index_val = i.arg(0);
    Value* fix_cmp = ops.check_if_fixnum(index_val);

    Value* tup = ops.upcast(rec, "Tuple");

    Value* index = ops.tag_strip32(index_val);
    Value* full_size = ops.get_tuple_size(tup);
    Value* size_cmp = ops.create_less_than(index, full_size, "is_in_bounds");

    // Combine fix_cmp and size_cmp to validate entry into access code
    Value* access_cmp = ops.create_and(fix_cmp, size_cmp, "access_cmp");
    ops.create_conditional_branch(access, is_other, access_cmp);

    ops.set_block(access);

    Value* value = i.arg(1);
    ops.stack_remove(3);

    Value* idx[] = {
      ConstantInt::get(Type::Int32Ty, 0),
      ConstantInt::get(Type::Int32Ty, offset::tuple_field),
      index
    };

    Value* gep = ops.create_gep(tup, idx, 3, "field_pos");

    ops.create_store(value, gep);
    ops.write_barrier(tup, value);
    ops.stack_push(value);

    ops.create_branch(i.after());

    ops.set_block(is_other);
  }

  static void fixnum_and(JITOperations& ops, Inliner& i) {
    Value* lint = ops.cast_int(i.recv());
    Value* rint = ops.cast_int(i.arg(0));

    Value* anded = BinaryOperator::CreateAnd(lint, rint, "fixnums_anded", ops.current_block());

    Value* fix_mask = ConstantInt::get(Type::Int32Ty, TAG_FIXNUM_MASK);
    Value* fix_tag  = ConstantInt::get(Type::Int32Ty, TAG_FIXNUM);

    Value* masked = BinaryOperator::CreateAnd(anded, fix_mask, "masked", ops.current_block());

    Value* cmp = ops.create_equal(masked, fix_tag, "is_fixnum");

    BasicBlock* push = ops.new_block("push_bit_and");
    BasicBlock* send = ops.new_block("send_bit_and");

    ops.create_conditional_branch(push, send, cmp);

    ops.set_block(push);
    ops.stack_remove(1);
    ops.stack_set_top(ops.as_obj(anded));
    ops.create_branch(i.after());

    ops.set_block(send);
  }

  static void fixnum_neg(JITOperations& ops, Inliner& i) {
    BasicBlock* use_send = ops.new_block("use_send");
    BasicBlock* inlined = ops.new_block("fixnum_neg");

    Value* self = i.recv();
    Value* cmp = ops.check_if_fixnum(self);
    ops.create_conditional_branch(inlined, use_send, cmp);

    ops.set_block(inlined);
    Value* native = ops.tag_strip32(self);
    Value* neg = BinaryOperator::CreateSub(
        ConstantInt::get(Type::Int32Ty, 0), native, "to_neg",
        ops.current_block());

    Value* one = ConstantInt::get(Type::Int32Ty, 1);
    Value* more = BinaryOperator::CreateShl(neg, one, "shl", ops.current_block());
    Value* tagged = BinaryOperator::CreateOr(more, one, "or", ops.current_block());

    ops.stack_set_top(ops.as_obj(tagged));
    ops.create_branch(i.after());

    ops.set_block(use_send);
  }

  static void object_equal(Class* klass, JITOperations& ops, Inliner& i) {
    Value* self = i.recv();

    BasicBlock* use_send = ops.new_block("use_send");
    ops.check_class(self, klass, use_send);

    Value* cmp = ops.create_equal(self, i.arg(0), "idenity_equal");
    Value* imm_value = SelectInst::Create(cmp, ops.constant(Qtrue),
          ops.constant(Qfalse), "select_bool", ops.current_block());

    ops.stack_remove(1);
    ops.stack_set_top(imm_value);
    ops.create_branch(i.after());

    ops.set_block(use_send);
  }

  bool Inliner::inline_primitive(Class* klass, CompiledMethod* cm, executor prim) {
    char* inlined_prim = 0;

    if(prim == Primitives::tuple_at && count_ == 1) {
      inlined_prim = "tuple_at";
      call_tuple_at(ops_, *this);
    } else if(prim == Primitives::tuple_put && count_ == 2) {
      inlined_prim = "tuple_put";
      call_tuple_put(ops_, *this);
    } else if(prim == Primitives::fixnum_and && count_ == 1) {
      inlined_prim = "fixnum_and";
      fixnum_and(ops_, *this);
    } else if(prim == Primitives::fixnum_neg && count_ == 0) {
      inlined_prim = "fixnum_neg";
      fixnum_neg(ops_, *this);
    } else if(prim == Primitives::object_equal && count_ == 1) {
      inlined_prim = "object_equal";
      object_equal(klass, ops_, *this);
    }

    if(inlined_prim) {
      if(ops_.state()->config().jit_inline_debug) {
        std::cerr << "inlining: "
          << ops_.state()->symbol_cstr(cm->scope()->module()->name())
          << "#"
          << ops_.state()->symbol_cstr(cm->name())
          << " into "
          << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
          << ". primitive " << inlined_prim << "\n";
      }

      return true;
    }

    // Add more primitive inlining!
    if(ops_.state()->config().jit_inline_debug) {
      std::cerr << "NOT inlining: "
        << ops_.state()->symbol_cstr(cm->scope()->module()->name())
        << "#"
        << ops_.state()->symbol_cstr(cm->name())
        << " into "
        << ops_.state()->symbol_cstr(ops_.vmmethod()->original->name())
        << ". primitive\n";
    }
    return false;
  }
}

#endif
