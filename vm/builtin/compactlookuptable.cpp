#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/compactlookuptable.hpp"
#include "builtin/lookuptable.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  void CompactLookupTable::init(STATE) {
    GO(compactlookuptable).set(state->new_class("CompactLookupTable",
          G(tuple), CompactLookupTable::fields));
    G(compactlookuptable)->set_object_type(CompactLookupTableType);
  }

  CompactLookupTable* CompactLookupTable::create(STATE) {
    return (CompactLookupTable*)state->om->new_object(G(compactlookuptable),
                                                      COMPACTLOOKUPTABLE_SIZE);
  }

  OBJECT CompactLookupTable::fetch(STATE, OBJECT key) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      if(at(i) == key) return at(i + 1);
    }

    return Qnil;
  }

  OBJECT CompactLookupTable::store(STATE, OBJECT key, OBJECT val) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      OBJECT tmp = at(i);
      if(tmp == key || tmp->nil_p()) {
        put(state, i, key);
        put(state, i + 1, val);
        return Qtrue;
      }
    }

    return Qfalse;
  }

  OBJECT CompactLookupTable::has_key(STATE, OBJECT key) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      if(at(i) == key) return Qtrue;
    }

    return Qfalse;
  }

  Array* CompactLookupTable::keys(STATE) {
    Array* ary = Array::create(state, COMPACTLOOKUPTABLE_SIZE / 2);

    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      OBJECT key = at(i);
      if(!key->nil_p()) ary->append(state, key);
    }

    return ary;
  }

  Array* CompactLookupTable::values(STATE) {
    Array* ary = Array::create(state, COMPACTLOOKUPTABLE_SIZE / 2);

    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      OBJECT key = at(i);
      if(!key->nil_p()) ary->append(state, at(i + 1));
    }

    return ary;
  }

  LookupTable* CompactLookupTable::to_lookuptable(STATE) {
    LookupTable* tbl = (LookupTable*)LookupTable::create(state);

    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      OBJECT key = at(i);
      if(!key->nil_p()) tbl->store(state, key, at(i + 1));
    }

    return tbl;
  }
}
