#include "vm.hpp"
#include "objectmemory.hpp"
#include "call_frame.hpp"

#include "builtin/object.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/class.hpp"

namespace rubinius {
  void VariableScope::init(STATE) {
    GO(variable_scope).set(state->new_class("VariableScope", G(object), G(rubinius)));
    G(variable_scope)->set_object_type(state, VariableScopeType);
    G(variable_scope)->name(state, state->symbol("Rubinius::VariableScope"));
  }

  VariableScope* VariableScope::promote(STATE) {
    VariableScope* scope = state->new_struct<VariableScope>(
        G(variable_scope), number_of_locals_ * sizeof(Object*));

    scope->block(state, block_);
    scope->exitted_ = exitted_;
    scope->method(state, method_);
    scope->module(state, module_);
    scope->parent(state, parent_);
    scope->self(state, self_);

    scope->number_of_locals_ = number_of_locals_;

    for(int i = 0; i < number_of_locals_; i++) {
      scope->set_local(state, i, locals_[i]);
    }

    return scope;
  }

  void VariableScope::setup_as_block(VariableScope* top, VariableScope* parent, CompiledMethod* cm, int num, Object* self) {
    init_header(UnspecifiedZone, InvalidType);

    parent_ = parent;
    if(self) {
      self_ = self;
    } else {
      self_ = parent->self();
    }

    method_ = cm;
    module_ = top->module();
    block_ =  top->block();
    number_of_locals_ = num;
    exitted_ = Qnil;

    for(int i = 0; i < num; i++) {
      locals_[i] = Qnil;
    }
  }

  VariableScope* VariableScope::of_sender(STATE, CallFrame* call_frame) {
    CallFrame* dest = static_cast<CallFrame*>(call_frame->previous);
    dest->promote_scope(state);
    return dest->scope;
  }

  VariableScope* VariableScope::current(STATE, CallFrame* call_frame) {
    call_frame->promote_scope(state);
    return call_frame->scope;
  }

  Tuple* VariableScope::locals(STATE) {
    Tuple* tup = Tuple::create(state, number_of_locals_);
    for(int i = 0; i < number_of_locals_; i++) {
      tup->put(state, i, locals_[i]);
    }

    return tup;
  }

  size_t VariableScope::Info::object_size(const ObjectHeader* obj) {
    const VariableScope* scope = reinterpret_cast<const VariableScope*>(obj);

    return sizeof(VariableScope) + (scope->number_of_locals_ * sizeof(Object*));
  }

  void VariableScope::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);

    Object* tmp;
    VariableScope* vs = as<VariableScope>(obj);

    size_t locals = vs->number_of_locals();
    for(size_t i = 0; i < locals; i++) {
      tmp = mark.call(vs->get_local(i));
      if(tmp) vs->set_local(mark.gc->object_memory->state, i, tmp);
    }
  }

  void VariableScope::Info::visit(Object* obj, ObjectVisitor& visit) {
    auto_visit(obj, visit);

    VariableScope* vs = as<VariableScope>(obj);

    size_t locals = vs->number_of_locals();
    for(size_t i = 0; i < locals; i++) {
      visit.call(vs->get_local(i));
    }
  }


}
