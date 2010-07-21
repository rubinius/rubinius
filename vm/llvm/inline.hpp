#include "llvm/jit_operations.hpp"
#include "llvm/access_memory.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"

namespace rubinius {

  class NativeFunction;

  namespace jit {
    class Context;
  }

  class Inliner {
    jit::Context& context_;
    JITOperations& ops_;
    InlineCache* cache_;
    int count_;
    int self_pos_;
    BasicBlock* failure_;
    Value* result_;
    bool check_for_exception_;
    JITInlineBlock* inline_block_;
    JITInlineBlock* block_info_;

    bool block_on_stack_;
    bool from_unboxed_array_;

    JITMethodInfo* creator_info_;

    bool fail_to_send_;

  public:

    Inliner(jit::Context& ctx,
            JITOperations& ops, InlineCache* cache, int count, BasicBlock* failure)
      : context_(ctx)
      , ops_(ops)
      , cache_(cache)
      , count_(count)
      , self_pos_(count)
      , failure_(failure)
      , result_(0)
      , check_for_exception_(true)
      , inline_block_(0)
      , block_info_(0)
      , block_on_stack_(false)
      , from_unboxed_array_(false)
      , creator_info_(0)
      , fail_to_send_(false)
    {}

    Inliner(jit::Context& ctx, JITOperations& ops, int count)
      : context_(ctx)
      , ops_(ops)
      , cache_(0)
      , count_(count)
      , failure_(0)
      , result_(0)
      , check_for_exception_(true)
      , inline_block_(0)
      , block_info_(0)
      , block_on_stack_(false)
      , from_unboxed_array_(false)
      , creator_info_(0)
    {}

    Value* recv() {
      return ops_.stack_back(self_pos_);
    }

    Value* arg(int which) {
      return ops_.stack_back(self_pos_ - (which + 1));
    }

    BasicBlock* failure() {
      return failure_;
    }

    Value* result() {
      return result_;
    }

    void set_result(Value* val) {
      result_ = val;
    }

    void exception_safe() {
      check_for_exception_ = false;
    }

    bool check_for_exception() {
      return check_for_exception_;
    }

    void set_inline_block(JITInlineBlock* ib) {
      inline_block_ = ib;
    }

    JITInlineBlock* inline_block() {
      return inline_block_;
    }

    void set_block_on_stack() {
      self_pos_++;
      block_on_stack_ = true;
    }

    void set_from_unboxed_array() {
      from_unboxed_array_ = true;
    }

    void set_creator(JITMethodInfo* home) {
      creator_info_ = home;
    }

    void set_block_info(JITInlineBlock* ib) {
      block_info_ = ib;
    }

    bool fail_to_send() {
      return fail_to_send_;
    }

    void use_send_for_failure() {
      fail_to_send_ = true;
    }

    bool consider();
    void inline_block(JITInlineBlock* ib, Value* self);

    void inline_generic_method(Class* klass, Module* mod, CompiledMethod* cm, VMMethod* vmm);

    bool detect_trivial_method(VMMethod* vmm, CompiledMethod* cm = 0);

    void inline_trivial_method(Class* klass, CompiledMethod* cm);

    void inline_ivar_write(Class* klass, AccessVariable* acc);

    void inline_ivar_access(Class* klass, AccessVariable* acc);

    bool inline_primitive(Class* klass, CompiledMethod* cm, executor prim);

    bool inline_ffi(Class* klass, NativeFunction* nf);

    void emit_inline_block(JITInlineBlock* ib, Value* val);

    int detect_jit_intrinsic(Class* klass, CompiledMethod* cm);
    void inline_intrinsic(Class* klass, CompiledMethod* cm, int which);
  };

}
