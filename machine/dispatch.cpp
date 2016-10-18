#include "arguments.hpp"
#include "class/module.hpp"
#include "class/class.hpp"
#include "class/symbol.hpp"
#include "class/executable.hpp"
#include "class/exception.hpp"
#include "dispatch.hpp"
#include "global_cache.hpp"
#include "lookup_data.hpp"

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
      state->vm()->set_method_missing_reason(reason);

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

  bool Dispatch::resolve(STATE, Symbol* name, LookupData& lookup) {
    return GlobalCache::resolve(state, name, *this, lookup);
  }
}
