#include "configuration.hpp"
#include "memory.hpp"

#include "class/block_environment.hpp"
#include "class/jit.hpp"
#include "class/list.hpp"

namespace rubinius {
  void JIT::bootstrap(STATE) {
    Module* jit = state->memory()->new_module<JIT>(state, G(rubinius), "JIT");
    GO(jit).set(jit);

    Class* cls = state->memory()->new_class<Class>(state, G(jit), "CompileRequest");
    G(jit)->compile_class(state, cls);

    G(jit)->compile_list(state, List::create(state));
  }

  void JIT::initialize(STATE, JIT* obj, Module* under, const char* name) {
    Module::initialize(state, obj, under, name);
  }

  Object* JIT::compile(STATE, Object* object, CompiledCode* code,
                       Object* block_environment)
  {
    if(!CBOOL(enabled_p(state))) return cFalse;

    BlockEnvironment* block_env = try_as<BlockEnvironment>(block_environment);
    if(!block_env) block_env = nil<BlockEnvironment>();

    return cTrue;
  }

  Object* JIT::enabled_p(STATE) {
    return RBOOL(state->configuration()->jit_enabled.value);
  }

  Object* JIT::compile_threshold(STATE) {
    // TODO: JIT
    return Integer::from(state, 0 /*state->configuration()->jit_threshold_compile*/);
  }

  Object* JIT::sync_set(STATE, Object* flag) {
    return cFalse;
    /* TODO: JIT
    state->configuration()->jit_sync.set(CBOOL(flag));
    return sync_get(state);
    */
  }

  Object* JIT::sync_get(STATE) {
    return cFalse;
    /* TODO: JIT
    return RBOOL(state->configuration()->jit_sync);
    */
  }

  Object* JIT::enable(STATE) {
    if(!CBOOL(enabled_p(state))) return cFalse;

    return cTrue;
  }

  Object* JIT::compile_soon(STATE, CompiledCode* code,
      Class* receiver_class, BlockEnvironment* block_env, bool is_block)
  {
    if(!CBOOL(enabled_p(state))) return cFalse;

    return cTrue;
  }

  Object* JIT::compile_callframe(STATE, CompiledCode* code,
      int primitive)
  {
    if(!CBOOL(enabled_p(state))) return cFalse;

    return cTrue;
  }

  Object* JIT::start_method_update(STATE) {
    if(!CBOOL(enabled_p(state))) return cFalse;

    return cTrue;
  }

  Object* JIT::end_method_update(STATE) {
    if(!CBOOL(enabled_p(state))) return cFalse;

    return cTrue;
  }

  JITCompileRequest* JITCompileRequest::create(STATE, CompiledCode* code,
      Class* receiver_class, int hits, BlockEnvironment* block_env, bool is_block)
  {
    JITCompileRequest* request =
      state->memory()->new_object<JITCompileRequest>(state, G(jit)->compile_class());

    request->method(state, code);
    request->receiver_class(state, receiver_class);
    request->block_env(state, block_env);
    request->hits(hits);
    request->is_block(is_block);
    request->waiter(NULL);

    return request;
  }
}
