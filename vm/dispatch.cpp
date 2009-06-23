#include "dispatch.hpp"
#include "arguments.hpp"
#include "lookup_data.hpp"

#include "builtin/module.hpp"
#include "builtin/class.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/symbol.hpp"
#include "builtin/executable.hpp"

namespace rubinius {
  Object* Dispatch::send(STATE, CallFrame* call_frame, Arguments& args) {
    LookupData lookup(args.recv(), args.recv()->lookup_begin(state));

    return send(state, call_frame, lookup, args);
  }

  Object* Dispatch::send(STATE, CallFrame* call_frame, LookupData& lookup,
                         Arguments& args) {
    Symbol* original_name = name;

    if(!GlobalCacheResolver::resolve(state, name, *this, lookup)) {
      method_missing = true;
      lookup.priv = true;
      assert(GlobalCacheResolver::resolve(state, G(sym_method_missing), *this, lookup));
    }

    if(method_missing) {
      args.unshift(state, original_name);
    }

    return method->execute(state, call_frame, *this, args);
  }
}
