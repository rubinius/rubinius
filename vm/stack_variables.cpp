#include "stack_variables.hpp"
#include "builtin/variable_scope.hpp"
#include "vmmethod.hpp"
#include "call_frame.hpp"

namespace rubinius {

  VariableScope* StackVariables::create_heap_alias(STATE,
      CallFrame* call_frame)
  {
    if(on_heap_) return on_heap_;

    VMMethod* vmm = call_frame->cm->backend_method_;
    VariableScope* scope = state->new_struct<VariableScope>(
        G(variable_scope), vmm->number_of_locals * sizeof(Object*));

    if(parent_) {
      scope->parent(state, parent_);
    } else {
      scope->parent(state, (VariableScope*)Qnil);
    }

    scope->self(state, self_);
    scope->block(state, block_);
    scope->module(state, module_);
    scope->method(state, call_frame->cm);

    scope->isolated_ = false;
    scope->number_of_locals_ = vmm->number_of_locals;

    scope->point_locals_to(locals_);

    on_heap_ = scope;

    return scope;
  }

  void StackVariables::flush_to_heap(STATE) {
    if(!on_heap_) return;

    on_heap_->isolated_ = true;
    on_heap_->locals_ = on_heap_->heap_locals_;

    for(int i = 0; i < on_heap_->number_of_locals_; i++) {
      on_heap_->set_local(state, i, locals_[i]);
    }
  }
}
