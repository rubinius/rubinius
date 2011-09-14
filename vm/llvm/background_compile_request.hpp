#ifndef VM_LLVM_BACKGROUND_COMPILE_REQUEST_HPP
#define VM_LLVM_BACKGROUND_COMPILE_REQUEST_HPP

#include "llvm/state.hpp"

#include "builtin/compiledmethod.hpp"
#include "object_utils.hpp"

namespace rubinius {
  class BlockEnvironment;
  class CompiledMethod;

  class BackgroundCompileRequest {
    TypedRoot<CompiledMethod*> method_;
    TypedRoot<Object*> mm_;
    bool is_block_;
    thread::Condition* waiter_;

  public:
    BackgroundCompileRequest(STATE, CompiledMethod* cm, Object* mm, bool is_block=false)
      : method_(state)
      , mm_(state)
      , is_block_(is_block)
      , waiter_(0)
    {
      method_.set(cm);
      mm_.set(mm);
    }

    VMMethod* vmmethod() {
      return method_->backend_method();
    }

    CompiledMethod* method() {
      return method_.get();
    }

    BlockEnvironment* block_env() {
      return try_as<BlockEnvironment>(mm_.get());
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
  };

}

#endif
