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

  Object* BlockWrapper::call(STATE, Task* task, CallFrame* call_frame, size_t args) {
    return block_->call(state, task, call_frame, args);
  }

  Object* BlockWrapper::call_prim(STATE, Executable* exec, CallFrame* call_frame, Task* task, Message& msg) {
    return block_->call(state, task, call_frame, msg);
  }
}
