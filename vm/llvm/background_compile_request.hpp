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
    Class* receiver_class_;
    BlockEnvironment* block_env_;

    utilities::thread::Condition* waiter_;
    int hits_;
    bool is_block_;

  public:
    BackgroundCompileRequest(STATE, CompiledCode* code, Class* receiver_class,
                             int hits, BlockEnvironment* block_env = NULL, bool is_block=false)
      : method_(code)
      , receiver_class_(receiver_class)
      , block_env_(block_env)
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
      return block_env_;
    }

    Class* receiver_class() {
      return receiver_class_;
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

    void set_receiver_class(Class* receiver_class) {
      receiver_class_ = receiver_class;
    }

    void set_block_env(BlockEnvironment* block_env) {
      block_env_ = block_env;
    }
  };

}

#endif
