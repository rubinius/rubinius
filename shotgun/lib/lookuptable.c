#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/array.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/lookuptable.h"

/* MINSIZE MUST be a power of 2 */
#define LOOKUPTABLE_MIN_SIZE 16
#define LOOKUPTABLE_MAX_DENSITY 0.75
#define LOOKUPTABLE_MIN_DENSITY 0.3

/* TODO: find out if this double cast is necessary. */
#define key_hash(obj) (((unsigned int)(uintptr_t)obj) >> 2)
#define find_bin(hash, bins) (hash & ((bins) - 1))
#define get_bins(tbl) lookuptable_get_bins(tbl)
#define get_values(tbl) lookuptable_get_values(tbl)
#define get_entries(tbl) lookuptable_get_entries(tbl)
#define max_density_p(ents,bins) (ents >= LOOKUPTABLE_MAX_DENSITY * bins)
#define min_density_p(ents,bins) (ents < LOOKUPTABLE_MIN_DENSITY * bins)
#define key_to_sym(key) \
  if(STRING_P(key)) { \
    key = symtbl_lookup(state, state->global->symbols, key); \
  } \

OBJECT lookuptable_new(STATE, int size) {
  OBJECT tbl;
  tbl = lookuptable_allocate(state);
  lookuptable_setup(state, tbl, size);
  return tbl;
}

OBJECT lookuptable_setup(STATE, OBJECT tbl, int size) {
  int sz;

  sz = size == 0 ? LOOKUPTABLE_MIN_SIZE : size;
  lookuptable_set_values(tbl, tuple_new(state, sz));
  lookuptable_set_bins(tbl, I2N(sz));
  lookuptable_set_entries(tbl, I2N(0));
  return tbl;
}

OBJECT lookuptable_dup(STATE, OBJECT tbl) {
  unsigned int i, bins, num;
  OBJECT new_tbl, entries, entry, key, value;

  bins = N2I(get_bins(tbl));
  num = N2I(get_entries(tbl));
  
  new_tbl = lookuptable_new(state, bins);
  SET_CLASS(new_tbl, object_class(state, tbl));

  entries = lookuptable_entries(state, tbl);
  for(i = 0; i < num; i++) {
    entry = array_get(state, entries, i);
    key = tuple_at(state, entry, 0);
    value = tuple_at(state, entry, 1);
    lookuptable_store(state, new_tbl, key, value);
  }
  return new_tbl;
}

static OBJECT new_entry(STATE, OBJECT key, OBJECT val) {
  OBJECT tup = tuple_new(state, 3);
  tuple_put(state, tup, 0, key);
  tuple_put(state, tup, 1, val);
  tuple_put(state, tup, 2, Qnil);
  return tup;
}

static void append_entry(STATE, OBJECT top, OBJECT entry) {
  OBJECT cur, next;

  cur = top;
  next = tuple_at(state, cur, 2);
  while(!NIL_P(next)) {
    cur = next;
    next = tuple_at(state, cur, 2);
  }
  tuple_put(state, cur, 2, entry);
}

static void redistribute(STATE, OBJECT tbl, unsigned int size) {
  unsigned int i, bin, bins;
  OBJECT values, new_values, entry, next, slot;

  bins = N2I(get_bins(tbl));
  values = get_values(tbl);
  new_values = tuple_new(state, size);

  for(i = 0; i < bins; i++) {
    entry = tuple_at(state, values, i);

    while(!NIL_P(entry)) {
      next = tuple_at(state, entry, 2);
      tuple_put(state, entry, 2, Qnil);

      bin = find_bin(key_hash(tuple_at(state, entry, 0)), size);
      slot = tuple_at(state, new_values, bin);

      if(NIL_P(slot)) {
        tuple_put(state, new_values, bin, entry);
      } else {
        append_entry(state, slot, entry);
      }

      entry = next;
    }
  }

  lookuptable_set_values(tbl, new_values);
  lookuptable_set_bins(tbl, I2N(size));
}

OBJECT lookuptable_store(STATE, OBJECT tbl, OBJECT key, OBJECT val) {
  unsigned int entries, bins, bin;
  OBJECT cur, entry, new_ent, values;

  key_to_sym(key);
  entries = N2I(get_entries(tbl));
  bins = N2I(get_bins(tbl));

  if(max_density_p(entries, bins)) {
    redistribute(state, tbl, bins <<= 1);
  }

  bin = find_bin(key_hash(key), bins);
  values = get_values(tbl);
  cur = entry = tuple_at(state, values, bin);

  while(!NIL_P(entry)) {
    if(tuple_at(state, entry, 0) == key) {
      tuple_put(state, entry, 1, val);
      return val;
    }
    cur = entry;
    entry = tuple_at(state, entry, 2);
  }

  new_ent = new_entry(state, key, val);
  if(NIL_P(cur)) {
    tuple_put(state, values, bin, new_ent);
  } else {
    tuple_put(state, cur, 2, new_ent);
  }
  lookuptable_set_entries(tbl, I2N(N2I(get_entries(tbl)) + 1));
  return val;
}

OBJECT lookuptable_fetch(STATE, OBJECT tbl, OBJECT key) {
  unsigned int bin;
  OBJECT entry;

  key_to_sym(key);
  bin = find_bin(key_hash(key), N2I(get_bins(tbl)));
  entry = tuple_at(state, get_values(tbl), bin);

  while(!NIL_P(entry)) {
    if(tuple_at(state, entry, 0) == key) {
      return tuple_at(state, entry, 1);
    }
    entry = tuple_at(state, entry, 2);
  }
  return Qnil;
}

OBJECT lookuptable_delete(STATE, OBJECT tbl, OBJECT key) {
  unsigned int entries, bins, bin;
  OBJECT cur, next, entry, val, values;

  key_to_sym(key);
  entries = N2I(get_entries(tbl));
  bins = N2I(get_bins(tbl));

  if(min_density_p(entries, bins)) {
    unsigned int new_bins = LOOKUPTABLE_MIN_SIZE;
    while(max_density_p(entries, new_bins)) {
      new_bins <<= 1;
    }
    redistribute(state, tbl, new_bins);
    bins = new_bins;
  }

  bin = find_bin(key_hash(key), bins);
  values = get_values(tbl);
  cur = entry = tuple_at(state, values, bin);

  while(!NIL_P(entry)) {
    next = tuple_at(state, entry, 2);

    if(tuple_at(state, entry, 0) == key) {
      val = tuple_at(state, entry, 1);
      if(entry == cur) {
        tuple_put(state, values, bin, next);
      } else {
        tuple_put(state, cur, 2, next);
      }
      lookuptable_set_entries(tbl, I2N(N2I(get_entries(tbl)) - 1));
      return val;
    }

    cur = entry;
    entry = next;
  }
  return Qnil;
}

OBJECT lookuptable_has_key(STATE, OBJECT tbl, OBJECT key) {
  unsigned int bin;
  OBJECT entry;

  key_to_sym(key);
  bin = find_bin(key_hash(key), N2I(get_bins(tbl)));
  entry = tuple_at(state, get_values(tbl), bin);

  while(!NIL_P(entry)) {
    if(tuple_at(state, entry, 0) == key) {
      return Qtrue;
    }
    entry = tuple_at(state, entry, 2);
  }
  return Qfalse;
}

static OBJECT collect(STATE, OBJECT tbl, OBJECT (*action)(STATE, OBJECT)) {
  unsigned int i, j, bins;
  OBJECT ary, values, entry;

  ary = array_new(state, N2I(get_entries(tbl)));
  bins = N2I(get_bins(tbl));
  values = get_values(tbl);

  for(i = j = 0; i < bins; i++) {
    entry = tuple_at(state, values, i);

    while(!NIL_P(entry)) {
      array_set(state, ary, j++, action(state, entry));
      entry = tuple_at(state, entry, 2);
    }
  }
  return ary;
}

static OBJECT get_key(STATE, OBJECT entry) {
  return tuple_at(state, entry, 0);
}

OBJECT lookuptable_keys(STATE, OBJECT tbl) {
  return collect(state, tbl, get_key);
}

static OBJECT get_value(STATE, OBJECT entry) {
  return tuple_at(state, entry, 1);
}

OBJECT lookuptable_values(STATE, OBJECT tbl) {
  return collect(state, tbl, get_value);
}

static OBJECT get_entry(STATE, OBJECT entry) {
  return entry;
}

OBJECT lookuptable_entries(STATE, OBJECT tbl) {
  return collect(state, tbl, get_entry);
}
