  intptr_t literal = next_int;
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
