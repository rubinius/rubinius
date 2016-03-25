#ifndef VM_LLVM_STACK_ARGS
#define VM_LLVM_STACK_ARGS

#include "jit/llvm/state.hpp"

#include <vector>

namespace rubinius {
  class JITStackArgs {
    bool from_unboxed_array_;
    size_t count_;
    std::vector<llvm::Value*> args_;

  public:
    JITStackArgs(size_t count)
      : from_unboxed_array_(false)
      , count_(count)
      , args_(count)
    {}

    void set_from_unboxed_array() {
      from_unboxed_array_ = true;
    }

    bool from_unboxed_array() {
      return from_unboxed_array_;
    }

    void put(size_t idx, llvm::Value* val) {
      if(idx < count_) {
        args_[idx] = val;
      }
    }

    llvm::Value* at(size_t idx) {
      if(idx < count_) {
        return args_[idx];
      }

      return 0;
    }

    size_t size() {
      return count_;
    }
  };
}

#endif
