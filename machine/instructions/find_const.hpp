#include "instructions.hpp"

#include "helpers.hpp"
#include "on_stack.hpp"

#include "class/autoload.hpp"
#include "class/constant_cache.hpp"

namespace rubinius {
  namespace instructions {
    inline bool find_const(STATE, CallFrame* call_frame, intptr_t literal) {
      Module* under = as<Module>(stack_pop());

      ConstantCache* cache = reinterpret_cast<ConstantCache*>(literal);

      Object* res = cache->retrieve(state, under, call_frame->lexical_scope());

      if(!res) {
        ConstantMissingReason reason;

        Symbol* sym = cache->name();
        res = Helpers::const_get_under(state, under, sym, &reason);

        if(reason == vFound) {
          OnStack<3> os(state, cache, under, res);
          if(Autoload* autoload = try_as<Autoload>(res)) {
            res = autoload->resolve(state, under);
          }

          if(res) {
            ConstantCache* update = ConstantCache::create(state, cache, res, under, call_frame->lexical_scope());
            cache->update_constant_cache(state, update);
          }
        } else {
          res = Helpers::const_missing_under(state, under, sym);
        }
      }

      CHECK_AND_PUSH(res);
    }
  }
}
