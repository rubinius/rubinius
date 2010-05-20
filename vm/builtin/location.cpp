#include "builtin/location.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/array.hpp"

#include "vm.hpp"

#include "call_frame.hpp"

namespace rubinius {
  void Location::init(STATE) {
    GO(location).set(state->new_class("Location", G(object), G(rubinius)));
    G(location)->set_object_type(state, LocationType);
    G(location)->name(state, state->symbol("Rubinius::Location"));
  }

  Location* Location::create(STATE, CallFrame* call_frame, bool include_variables) {
    Location* loc = state->new_object<Location>(G(location));
    loc->method_module(state, call_frame->module());
    loc->receiver(state, call_frame->self());
    loc->method(state, call_frame->cm);
    loc->ip(state, Fixnum::from(call_frame->ip() - 1));

    if(call_frame->is_block_p(state)) {
      loc->name(state, call_frame->top_scope(state)->method()->name());
      loc->is_block(state, Qtrue);
    } else {
      loc->name(state, call_frame->name());
      loc->is_block(state, Qfalse);
    }

    VMMethod* vmm = call_frame->cm->backend_method();
    if(vmm && vmm->jitted()) {
      loc->is_jit(state, Qtrue);
    } else {
      loc->is_jit(state, Qfalse);
    }

    if(include_variables) {
      loc->variables(state, call_frame->scope->create_heap_alias(state, call_frame));
    }

    loc->static_scope(state, call_frame->static_scope());

    return loc;
  }

  Array* Location::from_call_stack(STATE, CallFrame* call_frame, bool include_vars) {
    Array* bt = Array::create(state, 5);

    while(call_frame) {
      // Ignore synthetic frames
      if(call_frame->cm) {
        bt->append(state, Location::create(state, call_frame, include_vars));
      }

      call_frame = static_cast<CallFrame*>(call_frame->previous);
    }

    return bt;
  }
}
