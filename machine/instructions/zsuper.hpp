#include "instructions.hpp"

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
      intptr_t tup_index = 0;

      intptr_t fixed_args;
      if(splat) {
        fixed_args = mc->splat_position;
      } else if(mc->keywords) {
        fixed_args = mc->total_args - 1;
      } else {
        fixed_args = mc->total_args;
      }
      for(intptr_t i = 0; i < fixed_args; i++) {
        tup->put(state, tup_index++, scope->get_local(state, i));
      }

      if(splat) {
        for(intptr_t i = 0; i < splat->size(); i++) {
          tup->put(state, tup_index++, splat->get(state, i));
        }
      } else if(splat_obj) {
        tup->put(state, tup_index++, splat_obj);
      }

      if(mc->post_args) {
        intptr_t post_position = mc->splat_position + 1;
        for(intptr_t i = post_position; i < post_position + mc->post_args; i++) {
          tup->put(state, tup_index++, scope->get_local(state, i));
        }
      }

      if(mc->keywords) {
        intptr_t placeholder_position = splat_obj ? mc->total_args : mc->total_args - 1;
        intptr_t keywords_position    = placeholder_position + 1;

        // "Runtime.keyword_object?" requires keywords object to be convertable into Hash.
        // TODO define "machine/class/hash" and use Hash::create instead of "[[key, value], ..].to_h".

        Array* keywords = Array::create(state, mc->keywords_count);

        for(intptr_t index = 0; index < mc->keywords_count; index++) {
          intptr_t keyword_position = keywords_position + index;
          Symbol* name  = as<Symbol>(call_frame->compiled_code->local_names()->at(state, keyword_position));
          Object* value = scope->get_local(state, keyword_position);

          Array* keyword = Array::create(state, 2);
          keyword->set(state, 0, name);
          keyword->set(state, 1, value);

          keywords->set(state, index, keyword);
        }

        Object* keywords_hash = keywords->send(state, state->symbol("to_h"));

        // We can receive optional arguments from "**args" at "placeholder_position".
        // It can be "nil" or "Hash", we need to merge with it.

        Object* placeholder = scope->get_local(state, placeholder_position);
        if (!placeholder->nil_p()) {
          Array* merge_arguments = Array::create(state, 1);
          merge_arguments->set(state, 0, placeholder);
          keywords_hash = keywords_hash->send(state, state->symbol("merge"), merge_arguments);
        }

        tup->put(state, tup_index++, keywords_hash);
      }

      CallSite* call_site = reinterpret_cast<CallSite*>(literal);

      Arguments new_args(call_site->name(), recv, block, arg_count, 0);
      new_args.use_tuple(tup, arg_count);

      Symbol* current_name = call_frame->original_name();
      if(call_site->name() != current_name) {
        call_site->name(state, current_name);
      }

      Object* value = call_site->execute(state, new_args);

      CHECK_AND_PUSH(value);
    }
  }
}
