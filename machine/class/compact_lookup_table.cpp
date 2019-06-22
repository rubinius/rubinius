#include "object_utils.hpp"
#include "memory.hpp"

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/compact_lookup_table.hpp"
#include "class/lookup_table.hpp"
#include "class/symbol.hpp"

namespace rubinius {
  void CompactLookupTable::bootstrap(STATE) {
    GO(compactlookuptable).set(state->memory()->new_class<Class, CompactLookupTable>(
          state, G(tuple), G(rubinius), "CompactLookupTable"));
  }

  CompactLookupTable* CompactLookupTable::create(STATE) {
    CompactLookupTable* tbl = state->memory()->new_fields<CompactLookupTable>(
        state, G(compactlookuptable), COMPACTLOOKUPTABLE_SIZE);
    CompactLookupTable::initialize(state, tbl);

    return tbl;
  }

  Object* CompactLookupTable::fetch(STATE, Object* key) {
    bool found;
    return fetch(state, key, &found);
  }

  Object* CompactLookupTable::fetch(STATE, Object* key, bool* found) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      if(at(state, i)->equal_p(key)) {
        *found = true;
        return at(state, i + 1);
      }
    }

    return cNil;
  }

  Object* CompactLookupTable::remove(STATE, Object* key, bool* removed) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      if(at(state, i)->equal_p(key)) {
        Object* val = at(state, i + 1);

        put(state, i, cNil);
        put(state, i + 1, cNil);
        if(removed) *removed = true;
        return val;
      }
    }

    return cNil;
  }

  Object* CompactLookupTable::store(STATE, Object* key, Object* val) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      Object* tmp = at(state, i);
      if(tmp->equal_p(key) || tmp->nil_p()) {
        put(state, i, key);
        put(state, i + 1, val);
        return cTrue;
      }
    }

    return cFalse;
  }

  Object* CompactLookupTable::has_key(STATE, Object* key) {
    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      if(at(state, i)->equal_p(key)) return cTrue;
    }

    return cFalse;
  }

  Array* CompactLookupTable::keys(STATE) {
    ObjectMatchAll matcher;
    return filtered_keys(state, matcher);
  }

  Array* CompactLookupTable::filtered_keys(STATE, ObjectMatcher& match, Array* ary) {
    if(!ary) ary = Array::create(state, COMPACTLOOKUPTABLE_SIZE / 2);

    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      Object* key = at(state, i);
      if(!key->nil_p() && match.match_p(state, key)) ary->append(state, key);
    }

    return ary;
  }

  Array* CompactLookupTable::values(STATE) {
    Array* ary = Array::create(state, COMPACTLOOKUPTABLE_SIZE / 2);

    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      Object* key = at(state, i);
      if(!key->nil_p()) ary->append(state, at(state, i + 1));
    }

    return ary;
  }

  LookupTable* CompactLookupTable::to_lookuptable(STATE) {
    LookupTable* tbl = LookupTable::create(state);

    for(unsigned int i = 0; i < COMPACTLOOKUPTABLE_SIZE; i += 2) {
      Object* key = at(state, i);
      if(!key->nil_p()) tbl->store(state, key, at(state, i + 1));
    }

    return tbl;
  }

  void CompactLookupTable::Info::show(STATE, Object* self, int level) {
    CompactLookupTable* tbl = as<CompactLookupTable>(self);
    Array* keys = tbl->keys(state);
    size_t size = keys->size();

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    indent(++level);
    for(size_t i = 0; i < size; i++) {
      std::cout << ":" << as<Symbol>(keys->get(state, i))->debug_str(state) << ", ";
    }
    std::cout << std::endl;
    close_body(level);
  }
}
