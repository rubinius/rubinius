#include "llvm/jit_builder.hpp"

namespace rubinius {
namespace jit {
  class BlockBuilder : public Builder {
    llvm::Value* module_;

  public:
    BlockBuilder(LLVMState* ls, JITMethodInfo& info)
      : Builder(ls, info)
    {}

    void initialize_frame(int stack_size);
    void setup_block_scope();
    void setup();
  };
}
}
