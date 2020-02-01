#include "dispatch.hpp"
#include "arguments.hpp"
#include "lookup_data.hpp"

#include "class/alias.hpp"
#include "class/class.hpp"
#include "class/exception.hpp"
#include "class/executable.hpp"
#include "class/method_table.hpp"
#include "class/module.hpp"
#include "class/symbol.hpp"

#include <sstream>

namespace rubinius {
  Object* Dispatch::send(STATE, Arguments& args, MethodMissingReason reason) {
    LookupData lookup(args.recv(), args.recv()->lookup_begin(state), G(sym_protected));

    return send(state, lookup, args, reason);
  }

  Object* Dispatch::send(STATE, LookupData& lookup,
                         Arguments& args, MethodMissingReason reason)
  {
    Symbol* original_name = name;

    if(!resolve(state, name, lookup)) {
      state->set_method_missing_reason(reason);

      method_missing = reason;
      lookup.min_visibility = G(sym_private);
      if(!resolve(state, G(sym_method_missing), lookup)) {
        Exception::raise_no_method_error(state, args);
      }

      method_missing = reason;
    }

    if(method_missing) {
      args.unshift(state, original_name);
    }

    return method->execute(state, method, module, args);
  }

  static bool hierarchy_resolve(STATE, Symbol* name, Dispatch* msg, LookupData& lookup) {
    Module* module = lookup.from;
    MethodTableBucket* entry;
    bool skip_vis_check = false;

    do {

      if(module != module->origin()) {
        module = module->superclass();
      }

      entry = module->method_table()->find_entry(state, name);

      /* Nothing, there? Ok, keep looking. */
      if(!entry) goto keep_looking;

      /* A 'false' method means to terminate method lookup.
       * (eg. undef_method) */
      if(entry->undef_p(state)) {
        msg->method_missing = eNormal;
        return false;
      }

      /* If this was a private send, then we can handle use
       * any method seen. */
      if(lookup.min_visibility == G(sym_private) || skip_vis_check) {
        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks. If we are skipping visibility checks we
         * shouldn't update visibility anymore because the implementation
         * might have a different visibility than the original lookup.
         */
        if(!skip_vis_check) {
          msg->visibility = entry->visibility();
        }
        if(entry->method()->nil_p() && entry->method_id()->nil_p()) {
          skip_vis_check = true;
          goto keep_looking;
        }

        if(Alias* alias = try_as<Alias>(entry->method())) {
          if(alias->original_exec()->nil_p()) {
            name           = alias->original_name();
            msg->visibility = entry->visibility();
            skip_vis_check = true;
          } else {
            msg->method = alias->original_exec();
            msg->module = alias->original_module();
          }
        } else {
          msg->method = entry->get_method(state);
          msg->module = module;
        }

        if(msg->method) {
          if(entry->prediction()->nil_p()) {
            entry->prediction(state, Prediction::create(state));
          }
          msg->prediction = entry->prediction();
          return true;
        }
      } else {
        /* The method is private, but this wasn't a private send. */
        if(entry->private_p(state)) {
          msg->method_missing = ePrivate;
          return false;
        } else if(entry->protected_p(state)) {
          /* The method is protected, but it's not being called from
           * the same module, or we only want public methods. */
          Module* check_mod;
          if(IncludedModule* im = try_as<IncludedModule>(module)) {
            check_mod = im->module();
          } else {
            check_mod = module;
          }

          if(lookup.min_visibility == G(sym_public) && !lookup.recv->kind_of_p(state, check_mod)) {
            msg->method_missing = eProtected;
            return false;
          }
        }

        /* The method was callable, but we need to keep looking
         * for the implementation, so make the invocation bypass all further
         * visibility checks. If we are skipping visibility checks we
         * shouldn't update visibility anymore because the implementation
         * might have a different visibility than the original lookup.
         */
        if(!skip_vis_check) {
          msg->visibility = entry->visibility();
        }
        if(entry->method()->nil_p() && entry->method_id()->nil_p()) {
        // TODO: fix using method() == cNil for this
        // if(entry->method()->nil_p()) {
          skip_vis_check = true;
          goto keep_looking;
        }

        if(Alias* alias = try_as<Alias>(entry->method())) {
          if(alias->original_exec()->nil_p()) {
            name           = alias->original_name();
            msg->visibility = entry->visibility();
            skip_vis_check = true;
          } else {
            msg->method = alias->original_exec();
            msg->module = alias->original_module();
          }
        } else {
          msg->method = entry->get_method(state);
          msg->module = module;
        }

        if(msg->method) {
          if(entry->prediction()->nil_p()) {
            entry->prediction(state, Prediction::create(state));
          }
          msg->prediction = entry->prediction();
          return true;
        }
      }

keep_looking:
      module = module->superclass();

      /* No more places to look, we couldn't find it. */
      if(module->nil_p()) {
        msg->method_missing = eNormal;
        return false;
      }
    } while(1);

    rubinius::bug("Control flow bug in method lookup");
  }

  bool Dispatch::resolve(STATE, Symbol* name, LookupData& lookup) {
    if(hierarchy_resolve(state, name, this, lookup)) {
      return true;
    }

    return false;
  }
}
