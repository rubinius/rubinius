#include "llvm/jit_operations.hpp"
#include "llvm/access_memory.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"

namespace rubinius {

  class NativeFunction;

  class Inliner {
    JITOperations& ops_;
    InlineCache* cache_;
    int count_;
    int self_pos_;
    BasicBlock* failure_;
    Value* result_;
    bool check_for_exception_;
    VMMethod* passed_block_;

    bool block_on_stack_;

  public:

    Inliner(JITOperations& ops, InlineCache* cache, int count, BasicBlock* failure)
      : ops_(ops)
      , cache_(cache)
      , count_(count)
      , self_pos_(count)
      , failure_(failure)
      , result_(0)
      , check_for_exception_(true)
      , passed_block_(0)
      , block_on_stack_(false)
    {}

    Inliner(JITOperations& ops, int count)
      : ops_(ops)
      , cache_(0)
      , count_(count)
      , failure_(0)
      , result_(0)
      , check_for_exception_(true)
      , passed_block_(0)
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

    void set_passed_block(CompiledMethod* cm) {
      passed_block_ = cm->backend_method();
    }

    void set_block_on_stack() {
      self_pos_++;
      block_on_stack_ = true;
    }

    bool consider();
    void inline_block(VMMethod* vmm, Value* self);

    void inline_generic_method(Class* klass, VMMethod* vmm, bool pass_block=false);

    bool detect_trivial_method(CompiledMethod* cm);

    void inline_trivial_method(Class* klass, CompiledMethod* cm);

    void inline_ivar_write(Class* klass, AccessVariable* acc);

    void inline_ivar_access(Class* klass, AccessVariable* acc);

    bool inline_primitive(Class* klass, CompiledMethod* cm, executor prim);

    bool inline_ffi(Class* klass, NativeFunction* nf);

    void emit_inline_block(VMMethod* vmm, Value* val);
  };

}
