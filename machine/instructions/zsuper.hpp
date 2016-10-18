#include "interpreter/instructions.hpp"

#include "machine_code.hpp"

#include "class/call_site.hpp"
#include "class/variable_scope.hpp"

namespace rubinius {
  namespace instructions {
    inline bool zsuper(STATE, CallFrame* call_frame, intptr_t literal) {
      Object* block = stack_pop();
      Object* const recv = call_frame->self();

      VariableScope* scope = call_frame->method_scope(state);
      interp_assert(scope);

      MachineCode* mc = scope->method()->machine_code();
      Object* splat_obj = 0;
      Array* splat = 0;

      size_t arg_count = mc->total_args;

      if(mc->splat_position >= 0) {
        splat_obj = scope->get_local(state, mc->splat_position);
        splat = try_as<Array>(splat_obj);
        if(splat) {
          arg_count += splat->size();
        } else {
          arg_count++;
        }
      }

      Tuple* tup = Tuple::create(state, arg_count);
      native_int tup_index = 0;

      native_int fixed_args;
      if(splat) {
        fixed_args = mc->splat_position;
      } else if(mc->keywords) {
        fixed_args = mc->total_args - 1;
      } else {
        fixed_args = mc->total_args;
      }
      for(native_int i = 0; i < fixed_args; i++) {
        tup->put(state, tup_index++, scope->get_local(state, i));
      }

      if(splat) {
        for(native_int i = 0; i < splat->size(); i++) {
          tup->put(state, tup_index++, splat->get(state, i));
        }
      } else if(splat_obj) {
        tup->put(state, tup_index++, splat_obj);
      }

      if(mc->post_args) {
        native_int post_position = mc->splat_position + 1;
        for(native_int i = post_position; i < post_position + mc->post_args; i++) {
          tup->put(state, tup_index++, scope->get_local(state, i));
        }
      }

      if(mc->keywords) {
        native_int placeholder_position = splat_obj ? mc->total_args : mc->total_args - 1;
        native_int keywords_position = placeholder_position + 1;

        Object* placeholder = scope->get_local(state, placeholder_position);
        Array* ary = Array::create(state, 2);

        for(native_int i = keywords_position; i <= mc->keywords_count; i++) {
          ary->set(state, 0, as<Symbol>(call_frame->compiled_code->local_names()->at(state, i)));
          ary->set(state, 1, scope->get_local(state, i));

          placeholder->send(state, state->symbol("[]="), ary);
        }

        tup->put(state, tup_index++, scope->get_local(state, placeholder_position));
      }

      CallSite* call_site = reinterpret_cast<CallSite*>(literal);

      Arguments new_args(call_site->name(), recv, block, arg_count, 0);
      new_args.use_tuple(tup, arg_count);

      Object* ret;

      Symbol* current_name = call_frame->original_name();
      if(call_site->name() != current_name) {
        call_site->name(state, current_name);
      }

      ret = call_site->execute(state, new_args);

      state->vm()->checkpoint(state);

      CHECK_AND_PUSH(ret);
    }
  }
}
