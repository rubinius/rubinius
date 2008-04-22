#include "vm.hpp"
#include "objectmemory.hpp"

/* MINSIZE MUST be a power of 2 */
#define LOOKUPTABLE_MAX_DENSITY 0.75
#define LOOKUPTABLE_MIN_DENSITY 0.3

#define key_hash(obj) (((unsigned int)(uintptr_t)obj) >> 2)
#define find_bin(hash, bins) (hash & ((bins) - 1))
#define max_density_p(ents,bins) (ents >= LOOKUPTABLE_MAX_DENSITY * bins)
#define min_density_p(ents,bins) (ents < LOOKUPTABLE_MIN_DENSITY * bins)
#define key_to_sym(key) \
  if(String::is_a(key)) { \
    key = ((String*)key)->to_sym(state); \
  } \


namespace rubinius {
  LookupTable* LookupTable::create(STATE, size_t size) {
    LookupTable *tbl;

    tbl = (LookupTable*)state->om->new_object(G(lookuptable),
                                              LookupTable::fields);
    tbl->setup(state, size);

    return tbl;
  }

  MethodTable* MethodTable::create(STATE) {
    MethodTable *tbl;

    tbl = (MethodTable*)state->om->new_object(G(lookuptable),
                                              LookupTable::fields);
    tbl->setup(state, 0);

    return tbl;
  }

  void LookupTable::setup(STATE, size_t sz = 0) {
    if(!sz) sz = LOOKUPTABLE_MIN_SIZE;
    SET(this, values, Tuple::create(state, sz));
    SET(this, bins, Object::i2n(sz));
    SET(this, entries, Object::i2n(0));
  }

  LookupTable* LookupTable::dup(STATE) {
    size_t size, i;
    LookupTable *dup;

    size = bins->n2i();
    dup = (LookupTable*)LookupTable::create(state, size);
    state->om->set_class(dup, class_object(state));
    size_t num = entries->n2i();

    Array* entries = (Array*)all_entries(state);
    for(i = 0; i < num; i++) {
      OBJECT entry = entries->get(state, i);
      OBJECT key =   entry->at(0);
      OBJECT value = entry->at(1);
      dup->store(state, key, value);
    }
    return dup;

  }

  OBJECT LookupTable::entry_new(STATE, OBJECT key, OBJECT data) {
    Tuple* tup = Tuple::create(state, 3);
    tup->put(state, 0, key);
    tup->put(state, 1, data);
    tup->put(state, 2, Qnil);
    return tup;
  }

  OBJECT LookupTable::entry_append(STATE, OBJECT top, OBJECT nxt) {
    Tuple* cur = (Tuple*)top->at(2);
    Tuple* last = (Tuple*)top;

    while(!cur->nil_p()) {
      last = cur;
      cur = (Tuple*)cur->at(2);
    }

    last->put(state, 2, nxt);
    return nxt;
  }

  void LookupTable::redistribute(STATE, size_t size) {
    size_t num = bins->n2i();
    Tuple* new_values = Tuple::create(state, size);

    for(size_t i = 0; i < num; i++) {
      Tuple* entry = (Tuple*)values->at(i);

      while(!entry->nil_p()) {
        Tuple* next = (Tuple*)entry->at(2);
        entry->put(state, 2, Qnil);

        size_t bin = find_bin(key_hash(entry->at(0)), size);
        OBJECT slot = new_values->at(bin);

        if(slot->nil_p()) {
          new_values->put(state, bin, entry);
        } else {
          entry_append(state, slot, entry);
        }

        entry = next;
      }
    }

    SET(this, values, new_values);
    bins = Object::i2n(size);
  }

  OBJECT LookupTable::store(STATE, OBJECT key, OBJECT val) {
    unsigned int num_entries, num_bins, bin;
    OBJECT new_ent;
    Tuple* cur;
    Tuple* entry;

    key_to_sym(key);
    num_entries = entries->n2i();
    num_bins = bins->n2i();

    if(max_density_p(num_entries, num_bins)) {
      redistribute(state, num_bins <<= 1);
    }

    bin = find_bin(key_hash(key), num_bins);
    cur = entry = (Tuple*)values->at(bin);

    while(!entry->nil_p()) {
      if(entry->at(0) == key) {
        entry->put(state, 1, val);
        return val;
      }
      cur = entry;
      entry = (Tuple*)entry->at(2);
    }

    new_ent = entry_new(state, key, val);
    if(NIL_P(cur)) {
      values->put(state, bin, new_ent);
    } else {
      cur->put(state, 2, new_ent);
    }

    entries = Object::i2n(num_entries + 1);
    return val;
  }

  OBJECT LookupTable::find_entry(STATE, OBJECT key) {
    unsigned int bin;
    OBJECT entry;

    key_to_sym(key);
    bin = find_bin(key_hash(key), bins->n2i());
    entry = values->at(bin);

    while(!entry->nil_p()) {
      if(entry->at(0) == key) {
        return entry;
      }
      entry = entry->at(2);
    }
    return Qnil;
  }

  OBJECT LookupTable::fetch(STATE, OBJECT key) {
    OBJECT entry;

    entry = find_entry(state, key);
    if(!entry->nil_p()) {
      return entry->at(1);
    }
    return Qnil;
  }

  OBJECT LookupTable::fetch(STATE, OBJECT key, bool* found) {
    OBJECT entry;

    entry = find_entry(state, key);
    if(!entry->nil_p()) {
      *found = true;
      return entry->at(1);
    }

    *found = false;
    return Qnil;
  }

  /* lookuptable_find returns Qundef if there is not entry
   * referenced by 'key' in the LookupTable. This is useful
   * to distinguish x = {} from x = {:a => nil} and is used
   * in cpu.c in e.g. cpu_const_get_in_context.
   */
  OBJECT LookupTable::find(STATE, OBJECT key) {
    OBJECT entry;

    entry = find_entry(state, key);
    if(!entry->nil_p()) {
      return entry->at(1);
    }
    return Qundef;
  }

  OBJECT LookupTable::remove(STATE, OBJECT key) {
    hashval bin;
    OBJECT val;
    Tuple* entry;
    Tuple* lst;
    Tuple* lk;

    key_to_sym(key);

    size_t num_entries = entries->n2i();
    size_t num_bins = bins->n2i();

    if(min_density_p(num_entries, num_bins) && (num_bins >> 1) >= LOOKUPTABLE_MIN_SIZE) {
      redistribute(state, num_bins >>= 1);
    }

    bin = find_bin(key_hash(key), num_bins);
    entry = (Tuple*)values->at(bin);

    lst = (Tuple*)Qnil;

    while(!entry->nil_p()) {
      lk = (Tuple*)entry->at(2);

      if(entry->at(0) == key) {
        val = entry->at(1);
        if(lst->nil_p()) {
          values->put(state, bin, lk);
        } else {
          lst->put(state, 2, lk);
        }
        entries = Object::i2n(entries->n2i() - 1);
        return val;
      }

      lst = entry;
      entry = lk;
    }

    return Qnil;
  }

  OBJECT LookupTable::has_key(STATE, OBJECT key) {
    OBJECT entry;

    entry = find_entry(state, key);
    if(!entry->nil_p()) {
      return Qtrue;
    }
    return Qfalse;
  }

  Array* LookupTable::collect(STATE, LookupTable* tbl, OBJECT (*action)(STATE, OBJECT)) {
    size_t i, j;
    OBJECT values, entry;

    Array* ary = Array::create(state, tbl->entries->n2i());
    size_t num_bins = tbl->bins->n2i();
    values = tbl->values;

    for(i = j = 0; i < num_bins; i++) {
      entry = values->at(i);

      while(!entry->nil_p()) {
        ary->set(state, j++, action(state, entry));
        entry = entry->at(2);
      }
    }
    return ary;
  }

  OBJECT LookupTable::get_key(STATE, OBJECT entry) {
    return entry->at(0);
  }

  Array* LookupTable::all_keys(STATE) {
    return collect(state, this, get_key);
  }

  OBJECT LookupTable::get_value(STATE, OBJECT entry) {
    return entry->at(1);
  }

  Array* LookupTable::all_values(STATE) {
    return collect(state, this, get_value);
  }

  OBJECT LookupTable::get_entry(STATE, OBJECT entry) {
    return entry;
  }

  OBJECT LookupTable::all_entries(STATE) {
    return collect(state, this, get_entry);
  }

}
