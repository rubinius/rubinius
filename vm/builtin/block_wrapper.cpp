#include "prelude.hpp"
#include "builtin/block_wrapper.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "builtin/class.hpp"
#include "builtin/block_environment.hpp"

namespace rubinius {

  void BlockWrapper::init(STATE) {
    Class* cls = state->new_class("Proc", G(object));
    GO(block_wrapper).set(
        state->new_class("FromBlock", cls, cls));
    G(block_wrapper)->set_object_type(state, BlockEnvironmentType);
    G(block_wrapper)->name(state, state->symbol("Proc::FromBlock"));
  }

  BlockWrapper* BlockWrapper::create(STATE, Object* self) {
    return state->new_object<BlockWrapper>(as<Class>(self));
  }

  BlockWrapper* BlockWrapper::from_env(STATE, BlockEnvironment* env) {
    BlockWrapper* wrapper = BlockWrapper::create(state, G(block_wrapper));
    wrapper->block(state, env);
    return wrapper;
  }

  void BlockWrapper::call(STATE, Task* task, size_t args) {
    block_->call(state, task, args);
  }

  ExecuteStatus BlockWrapper::call_prim(STATE, Executable* exec, Task* task, Message& msg) {
    block_->call(state, task, msg);
    return cExecuteRestart;
  }
}
