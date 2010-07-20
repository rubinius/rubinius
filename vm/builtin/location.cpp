#include "builtin/location.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/array.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"

#include "vm.hpp"

#include "call_frame.hpp"

#include <sstream>

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
      // Use promote_scope because it can figure out of the generated
      // VariableScope should be isolated by default (true atm for JITd
      // frames)
      loc->variables(state, call_frame->promote_scope(state));
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

  Array* Location::mri_backtrace(STATE, CallFrame* call_frame) {
    size_t count = 0;

    CallFrame* c = call_frame;
    while(c) {
      if(c->cm) count++;
      c = c->previous;
    }

    Array* bt = Array::create(state, count);

    while(call_frame) {
      // Ignore synthetic frames
      if(call_frame->cm) {
        std::stringstream ss;
        ss << call_frame->cm->file()->c_str(state)
           << ":"
           << call_frame->line(state)
           << ":in `";

        if(call_frame->block_p()) {
          ss << call_frame->top_scope(state)->method()->name()->c_str(state)
             << " {}'";
        } else {
          Symbol* current_name = call_frame->name();
          Symbol* method_name  = call_frame->cm->name();

          if(current_name->nil_p()) {
            if(method_name->nil_p()) {
              ss << "<unknown>";
            } else {
              ss << method_name->c_str(state);
            }
          } else if(current_name != method_name) {
            ss << current_name->c_str(state)
               << " ("
               << method_name->c_str(state)
               << ")";
          } else {
            ss << current_name->c_str(state);
          }

          ss << "'";
        }

        bt->append(state, String::create(state, ss.str().c_str()));
      }

      call_frame = static_cast<CallFrame*>(call_frame->previous);
    }

    return bt;
  }
}
