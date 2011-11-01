#include "dispatch.hpp"
#include "arguments.hpp"
#include "lookup_data.hpp"
#include "global_cache.hpp"

#include "builtin/module.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"
#include "builtin/executable.hpp"

namespace rubinius {
  Object* Dispatch::send(STATE, CallFrame* call_frame, Arguments& args,
                         MethodMissingReason reason)
  {
    LookupData lookup(args.recv(), args.recv()->lookup_begin(state));

    return send(state, call_frame, lookup, args, reason);
  }

  Object* Dispatch::send(STATE, CallFrame* call_frame, LookupData& lookup,
                         Arguments& args, MethodMissingReason reason)
  {
    Symbol* original_name = name;

    if(!GlobalCache::resolve(state, name, *this, lookup)) {
      state->vm()->set_method_missing_reason(reason);

      method_missing = true;
      lookup.priv = true;
      if(!GlobalCache::resolve(state, G(sym_method_missing), *this, lookup)) {
        Exception::internal_error(state, call_frame, "no method_missing");
        return 0;
      }

      method_missing = true;
    }

    if(method_missing) {
      args.unshift(state, original_name);
    }

    return method->execute(state, call_frame, method, module, args);
  }
}
