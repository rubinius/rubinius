#include "builtin/compactlookuptable.hpp"
#include "builtin/lookuptable.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  CompactLookupTable* CompactLookupTable::create(STATE) {
    return (CompactLookupTable*)state->om->new_object(G(compactlookuptable),
                                                      COMPACTLOOKUPTABLE_SIZE);
  }

  OBJECT CompactLookupTable::fetch(STATE, OBJECT key) {
    unsigned int i;

    for(i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      if(at(i) == key) {
        return at(i + 1);
      }
    }

    return Qnil;
  }

  OBJECT CompactLookupTable::store(STATE, OBJECT key, OBJECT val) {
    unsigned int i;
    OBJECT tmp;

    for(i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      tmp = at(i);
      if(tmp == key || tmp->nil_p()) {
        put(state, i, key);
        put(state, i + 1, val);
        return Qtrue;
      }
    }

    return Qfalse;
  }

  LookupTable* CompactLookupTable::to_lookuptable(STATE) {
    unsigned int i;
    OBJECT key;

    LookupTable* tbl = (LookupTable*)LookupTable::create(state);

    for(i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      key = at(i);
      if(!key->nil_p()) {
        tbl->store(state, key, at(i + 1));
      }
    }

    return tbl;
  }
}
