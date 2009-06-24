#include "llvm/jit_operations.hpp"

namespace rubinius {
  class Inliner {
    JITOperations& ops_;
    InlineCache* cache_;
    int count_;
    BasicBlock* after_;

  public:

    Inliner(JITOperations& ops, InlineCache* cache, int count, BasicBlock* after)
      : ops_(ops)
      , cache_(cache)
      , count_(count)
      , after_(after)
    {}

    Value* recv() {
      return ops_.stack_back(count_);
    }

    Value* arg(int which) {
      return ops_.stack_back(count_ - which);
    }

    void consider() {
      executor callee = cache_->method->execute;

      if(callee == Primitives::tuple_at && count_ == 1) {
        call_tuple_at();
      } else if(callee == Primitives::tuple_put && count_ == 2) {
        call_tuple_put();
      }
    }

    void call_tuple_at() {
      Value* rec = recv();

      // bool is_tuple = recv->flags & mask;

      Value* cmp = ops_.check_type_bits(rec, rubinius::Tuple::type);

      BasicBlock* is_tuple = ops_.new_block("is_tuple");
      BasicBlock* access =   ops_.new_block("tuple_at");
      BasicBlock* is_other = ops_.new_block("is_other");

      ops_.create_conditional_branch(is_tuple, is_other, cmp);

      ops_.set_block(is_tuple);

      Value* index_val = arg(0);

      Value* fix_cmp = ops_.check_if_fixnum(index_val);
      // Check that index is not over the end of the Tuple

      Value* tup = ops_.upcast(rec, "Tuple");

      Value* index = ops_.tag_strip32(index_val);
      Value* full_size = ops_.get_tuple_size(tup);
      Value* size_cmp = ops_.create_less_than(index, full_size, "is_in_bounds");

      // Combine fix_cmp and size_cmp to validate entry into access code
      Value* access_cmp = ops_.create_and(fix_cmp, size_cmp, "access_cmp");
      ops_.create_conditional_branch(access, is_other, access_cmp);

      ops_.set_block(access);

      ops_.stack_remove(2);

      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::tuple_field),
        index
      };

      Value* gep = ops_.create_gep(tup, idx, 3, "field_pos");

      ops_.stack_push(ops_.create_load(gep, "tuple_at"));

      ops_.create_branch(after_);

      ops_.set_block(is_other);
    }

    void call_tuple_put() {
      Value* rec = recv();

      Value* cmp = ops_.check_type_bits(rec, rubinius::Tuple::type);

      BasicBlock* is_tuple = ops_.new_block("is_tuple");
      BasicBlock* access =   ops_.new_block("tuple_put");
      BasicBlock* is_other = ops_.new_block("is_other");

      ops_.create_conditional_branch(is_tuple, is_other, cmp);

      ops_.set_block(is_tuple);

      Value* index_val = arg(0);
      Value* fix_cmp = ops_.check_if_fixnum(index_val);

      Value* tup = ops_.upcast(rec, "Tuple");

      Value* index = ops_.tag_strip32(index_val);
      Value* full_size = ops_.get_tuple_size(tup);
      Value* size_cmp = ops_.create_less_than(index, full_size, "is_in_bounds");

      // Combine fix_cmp and size_cmp to validate entry into access code
      Value* access_cmp = ops_.create_and(fix_cmp, size_cmp, "access_cmp");
      ops_.create_conditional_branch(access, is_other, access_cmp);

      ops_.set_block(access);

      Value* value = arg(1);
      ops_.stack_remove(3);

      Value* idx[] = {
        ConstantInt::get(Type::Int32Ty, 0),
        ConstantInt::get(Type::Int32Ty, offset::tuple_field),
        index
      };

      Value* gep = ops_.create_gep(tup, idx, 3, "field_pos");

      ops_.create_store(value, gep);
      ops_.write_barrier(tup, value);
      ops_.stack_push(value);

      ops_.create_branch(after_);

      ops_.set_block(is_other);
    }
  };

}
