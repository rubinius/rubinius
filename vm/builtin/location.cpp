#include "builtin/location.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "vm.hpp"

#include "call_frame.hpp"

namespace rubinius {
  void Location::init(STATE) {
    GO(location).set(state->new_class("Location", G(object), G(rubinius)));
    G(location)->set_object_type(state, LocationType);
    G(location)->name(state, state->symbol("Rubinius::Location"));
  }

  Location* Location::create(STATE, CallFrame* call_frame) {
    Location* loc = state->new_object<Location>(G(location));
    loc->method_module(state, call_frame->module());
    loc->receiver(state, call_frame->self());
    loc->method(state, call_frame->cm);
    loc->ip(state, Fixnum::from(call_frame->ip - 1));

    if(call_frame->is_block_p(state)) {
      loc->name(state, call_frame->top_scope()->method()->name());
      loc->is_block(state, Qtrue);
    } else {
      loc->name(state, call_frame->name());
      loc->is_block(state, Qfalse);
    }

    VMMethod* vmm = call_frame->cm->backend_method_;
    if(vmm && vmm->jitted()) {
      loc->is_jit(state, Qtrue);
    } else {
      loc->is_jit(state, Qfalse);
    }

    return loc;
  }
}
