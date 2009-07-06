#include "llvm/jit_operations.hpp"
#include "llvm/access_memory.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/module.hpp"

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
      return ops_.stack_back(count_ - (which + 1));
    }

    bool consider();

    void inline_generic_method(Class* klass, VMMethod* vmm);

    bool detect_trivial_method(CompiledMethod* cm);

    void inline_trivial_method(Class* klass, CompiledMethod* cm);

    void inline_ivar_write(Class* klass, AccessVariable* acc);

    void inline_ivar_access(Class* klass, AccessVariable* acc);

    void call_tuple_at();

    void call_tuple_put();
  };

}
