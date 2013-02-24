#ifndef VM_LLVM_BACKGROUND_COMPILE_REQUEST_HPP
#define VM_LLVM_BACKGROUND_COMPILE_REQUEST_HPP

#include "llvm/state.hpp"

#include "builtin/compiledcode.hpp"
#include "builtin/class.hpp"

#include "object_utils.hpp"

namespace rubinius {
  class BlockEnvironment;
  class CompiledCode;

  class BackgroundCompileRequest {
    CompiledCode* method_;
    Object* extra_;

    utilities::thread::Condition* waiter_;
    int hits_;
    bool is_block_;

  public:
    BackgroundCompileRequest(STATE, CompiledCode* code, Object* extra, int hits, bool is_block=false)
      : method_(code)
      , extra_(extra)
      , waiter_(0)
      , hits_(hits)
      , is_block_(is_block)
    {}

    MachineCode* machine_code() {
      return method_->machine_code();
    }

    CompiledCode* method() {
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

    int hits() {
      return hits_;
    }

    void set_waiter(utilities::thread::Condition* cond) {
      waiter_ = cond;
    }

    utilities::thread::Condition* waiter() {
      return waiter_;
    }

    void set_method(CompiledCode* meth) {
      method_ = meth;
    }

    void set_extra(Object* x) {
      extra_ = x;
    }
  };

}

#endif
