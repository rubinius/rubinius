#include "stack_variables.hpp"
#include "builtin/variable_scope.hpp"
#include "vmmethod.hpp"
#include "call_frame.hpp"

namespace rubinius {

  VariableScope* StackVariables::create_heap_alias(STATE,
      CallFrame* call_frame, bool full)
  {
    if(on_heap_) return on_heap_;

    VMMethod* vmm = call_frame->cm->backend_method();
    VariableScope* scope = state->new_object<VariableScope>(G(variable_scope));

    if(parent_) {
      scope->parent(state, parent_);
    } else {
      scope->parent(state, (VariableScope*)Qnil);
    }

    scope->self(state, self_);
    scope->block(state, block_);
    scope->module(state, module_);
    scope->method(state, call_frame->cm);
    scope->heap_locals(state, Tuple::create(state, vmm->number_of_locals));

    scope->number_of_locals_ = vmm->number_of_locals;

    if(full) {
      scope->isolated_ = false;
    } else {
      scope->isolated_ = true;
    }

    scope->locals_ = locals_;

    scope->set_block_as_method(call_frame->block_as_method_p());

    on_heap_ = scope;

    return scope;
  }

  void StackVariables::flush_to_heap(STATE) {
    if(!on_heap_) return;

    on_heap_->isolated_ = true;

    for(int i = 0; i < on_heap_->number_of_locals_; i++) {
      on_heap_->set_local(state, i, locals_[i]);
    }
  }
}
