#ifndef VM_LLVM_BACKGROUND_COMPILE_REQUEST_HPP
#define VM_LLVM_BACKGROUND_COMPILE_REQUEST_HPP

#include "llvm/state.hpp"

#include "builtin/compiledmethod.hpp"
#include "builtin/class.hpp"

#include "object_utils.hpp"

namespace rubinius {
  class BlockEnvironment;
  class CompiledMethod;

  class BackgroundCompileRequest {
    CompiledMethod* method_;
    Object* extra_;

    bool is_block_;
    thread::Condition* waiter_;

  public:
    BackgroundCompileRequest(STATE, CompiledMethod* cm, Object* extra, bool is_block=false)
      : method_(cm)
      , extra_(extra)
      , is_block_(is_block)
      , waiter_(0)
    {}

    VMMethod* vmmethod() {
      return method_->backend_method();
    }

    CompiledMethod* method() {
      return method_;
    }

    BlockEnvironment* block_env() {
      return try_as<BlockEnvironment>(extra_);
    }

    Class* receiver_class() {
      return try_as<Class>(extra_);
    }

    Object* extra() {
      return extra_;
    }

    bool is_block() {
      return is_block_;
    }

    void set_waiter(thread::Condition* cond) {
      waiter_ = cond;
    }

    thread::Condition* waiter() {
      return waiter_;
    }

    void set_method(CompiledMethod* meth) {
      method_ = meth;
    }

    void set_extra(Object* x) {
      extra_ = x;
    }
  };

}

#endif
