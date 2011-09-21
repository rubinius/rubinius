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
    TypedRoot<CompiledMethod*> method_;
    TypedRoot<Object*> extra_;
    bool is_block_;
    thread::Condition* waiter_;

  public:
    BackgroundCompileRequest(STATE, CompiledMethod* cm, Object* extra, bool is_block=false)
      : method_(state)
      , extra_(state)
      , is_block_(is_block)
      , waiter_(0)
    {
      method_.set(cm);
      extra_.set(extra);
    }

    VMMethod* vmmethod() {
      return method_->backend_method();
    }

    CompiledMethod* method() {
      return method_.get();
    }

    BlockEnvironment* block_env() {
      return try_as<BlockEnvironment>(extra_.get());
    }

    Class* receiver_class() {
      return try_as<Class>(extra_.get());
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
