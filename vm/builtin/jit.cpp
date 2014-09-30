#include "builtin/jit.hpp"
#include "builtin/list.hpp"

#include "ontology.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#endif

namespace rubinius {
  void JIT::init(STATE) {
    Module* jit = state->new_object<JIT>(G(module));
    jit->setup(state, "JIT", G(rubinius));
    GO(jit).set(jit);

    Class* cls = ontology::new_class_under(state, "CompileRequest", G(jit));
    G(jit)->compile_class(state, cls);

    G(jit)->compile_list(state, List::create(state));
  }

  Object* JIT::enable(STATE) {
    state->shared().llvm_state->enable(state);
    enabled(state, cTrue);

    return cTrue;
  }

  JITCompileRequest* JITCompileRequest::create(STATE, CompiledCode* code,
      Class* receiver_class, int hits, BlockEnvironment* block_env, bool is_block)
  {
    JITCompileRequest* request = state->new_object<JITCompileRequest>(
        G(jit)->compile_class());
    request->method(state, code);
    request->receiver_class(state, receiver_class);
    request->block_env(state, block_env);
    request->hits_ = hits;
    request->is_block_ = is_block;
    request->waiter_ = NULL;

    return request;
  }


}
