#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/location.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/variable_scope.hpp"
#include "call_frame.hpp"
#include "ontology.hpp"

namespace rubinius {
  void Location::init(STATE) {
    GO(location).set(ontology::new_class(state, "Location",
          G(object), G(rubinius)));
    G(location)->set_object_type(state, LocationType);
  }

  Location* Location::create(STATE, CallFrame* call_frame,
                             bool include_variables)
  {
    if(NativeMethodFrame* nmf = call_frame->native_method_frame()) {
      return create(state, nmf);
    }

    Location* loc = state->new_object_dirty<Location>(G(location));
    loc->method_module(state, call_frame->module());
    loc->receiver(state, call_frame->self());
    loc->method(state, call_frame->compiled_code);
    loc->ip(state, Fixnum::from(call_frame->ip()));
    loc->flags(state, Fixnum::from(0));

    if(call_frame->is_block_p(state)) {
      loc->name(state, call_frame->top_scope(state)->method()->name());
      loc->set_is_block(state);
    } else {
      loc->name(state, call_frame->name());
    }

    MachineCode* mcode = call_frame->compiled_code->machine_code();
    if(mcode && mcode->jitted()) {
      loc->set_is_jit(state);
    }

    if(include_variables) {
      // Use promote_scope because it can figure out of the generated
      // VariableScope should be isolated by default (true atm for JITd
      // frames)
      loc->variables(state, call_frame->promote_scope(state));
    } else {
      loc->variables(state, nil<VariableScope>());
    }

    loc->constant_scope(state, call_frame->constant_scope());

    return loc;
  }

  Location* Location::of_closest_ruby_method(STATE, CallFrame* calling_environment) {
    CallFrame* dest = calling_environment->previous;
    // Skip any frames for native methods
    while(dest->native_method_p()) { dest = dest->previous; }
    return Location::create(state, dest, false);
  }

  Location* Location::create(STATE, NativeMethodFrame* nmf) {
    NativeMethod* nm = try_as<NativeMethod>(nmf->get_object(nmf->method()));
    if(!nm) return 0;

    Location* loc = state->new_object<Location>(G(location));
    if(Module* mod = try_as<Module>(nmf->get_object(nmf->module()))) {
      loc->method_module(state, mod);
    }

    loc->receiver(state, nmf->get_object(nmf->receiver()));

    loc->method(state, nm);
    loc->ip(state, Fixnum::from(-1));
    loc->flags(state, Fixnum::from(2));

    loc->name(state, nm->name());
    return loc;
  }

  Array* Location::from_call_stack(STATE, CallFrame* start_call_frame,
                                   bool include_vars, bool on_ip)
  {
    Array* bt = Array::create(state, 5);
    CallFrame* call_frame = start_call_frame;

    // Initial edge.
    if(!call_frame) return bt;

    // First the first normal frame
    while(!call_frame->compiled_code) {
      call_frame = call_frame->previous;
      // Weird edge case.
      if(!call_frame) return bt;
    }

    Location* loc = Location::create(state, call_frame, include_vars);
    if(on_ip) loc->set_ip_on_current(state);

    bt->append(state, loc);

    call_frame = call_frame->previous;

    while(call_frame) {
      // Ignore synthetic frames
      if(call_frame->compiled_code) {
        bt->append(state, Location::create(state, call_frame, include_vars));
      } else if(NativeMethodFrame* nmf = call_frame->native_method_frame()) {
        Location* loc = Location::create(state, nmf);
        if(loc) bt->append(state, loc);
      }

      call_frame = call_frame->previous;
    }

    return bt;
  }

  Array* Location::mri_backtrace(STATE, CallFrame* call_frame) {
    size_t count = 0;

    CallFrame* c = call_frame;
    while(c) {
      if(c->compiled_code) count++;
      c = c->previous;
    }

    Array* bt = Array::create(state, count);

    while(call_frame) {
      // Ignore synthetic frames
      if(call_frame->compiled_code &&
         !call_frame->compiled_code->kernel_method(state)) {
        Symbol* name;
        Object* block = cFalse;
        Fixnum* line = Fixnum::from(call_frame->line(state));

        if(call_frame->block_p()) {
          block = cTrue;
          name = call_frame->top_scope(state)->method()->name();
        } else {
          Symbol* current_name = call_frame->name();
          Symbol* method_name  = call_frame->compiled_code->name();

          if(current_name->nil_p()) {
            if(method_name->nil_p()) {
              name = state->symbol("<unknown>");
            } else {
              name = method_name;
            }
          } else {
            name = current_name;
          }
        }

        bt->append(state,
            Tuple::from(state, 4, call_frame->compiled_code, line, block, name));
      }

      call_frame = call_frame->previous;
    }

    return bt;
  }
}
