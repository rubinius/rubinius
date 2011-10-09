#include "builtin/location.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/array.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"
#include "builtin/nativemethod.hpp"

#include "vm.hpp"

#include "call_frame.hpp"

#include <sstream>

namespace rubinius {
  void Location::init(STATE) {
    GO(location).set(state->new_class("Location", G(object), G(rubinius)));
    G(location)->set_object_type(state, LocationType);
    G(location)->name(state, state->symbol("Rubinius::Location"));
  }

  Location* Location::create(STATE, CallFrame* call_frame,
                             bool include_variables)
  {
    if(NativeMethodFrame* nmf = call_frame->native_method_frame()) {
      return create(state, nmf);
    }

    Location* loc = state->new_object<Location>(G(location));
    loc->method_module(state, call_frame->module());
    loc->receiver(state, call_frame->self());
    loc->method(state, call_frame->cm);
    loc->ip(state, Fixnum::from(call_frame->ip()));
    loc->flags(state, Fixnum::from(0));

    if(call_frame->is_block_p(state)) {
      loc->name(state, call_frame->top_scope(state)->method()->name());
      loc->set_is_block(state);
    } else {
      loc->name(state, call_frame->name());
    }

    VMMethod* vmm = call_frame->cm->backend_method();
    if(vmm && vmm->jitted()) {
      loc->set_is_jit(state);
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
    while(!call_frame->cm) {
      call_frame = static_cast<CallFrame*>(call_frame->previous);
      // Weird edge case.
      if(!call_frame) return bt;
    }

    Location* loc = Location::create(state, call_frame, include_vars);
    if(on_ip) loc->set_ip_on_current(state);

    bt->append(state, loc);

    call_frame = static_cast<CallFrame*>(call_frame->previous);

    while(call_frame) {
      // Ignore synthetic frames
      if(call_frame->cm) {
        bt->append(state, Location::create(state, call_frame, include_vars));
      } else if(NativeMethodFrame* nmf = call_frame->native_method_frame()) {
        Location* loc = Location::create(state, nmf);
        if(loc) bt->append(state, loc);
      }

      call_frame = static_cast<CallFrame*>(call_frame->previous);
    }

    return bt;
  }

  static bool kernel_method(STATE, CompiledMethod* cm) {
    std::string s = cm->file()->cpp_str(state);
    if(s.size() >= 7 && strncmp(s.data(), "kernel/", 7) == 0) return true;
    return false;
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
      if(call_frame->cm && !kernel_method(state, call_frame->cm)) {
        Symbol* name;
        Object* block = Qfalse;
        Fixnum* line = Fixnum::from(call_frame->line(state));

        if(call_frame->block_p()) {
          block = Qtrue;
          name = call_frame->top_scope(state)->method()->name();
        } else {
          Symbol* current_name = call_frame->name();
          Symbol* method_name  = call_frame->cm->name();

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
            Tuple::from(state, 4, call_frame->cm, line, block, name));
      }

      call_frame = static_cast<CallFrame*>(call_frame->previous);
    }

    return bt;
  }
}
