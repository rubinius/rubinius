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

  bool Inliner::inline_primitive(Class* klass, CompiledMethod* cm, executor prim) {
    char* inlined_prim = 0;

    if(prim == Primitives::tuple_at && count_ == 1) {
      inlined_prim = "tuple_at";
      call_tuple_at(ops_, *this);
    } else if(prim == Primitives::tuple_put && count_ == 2) {
      inlined_prim = "tuple_put";
      call_tuple_put(ops_, *this);
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
