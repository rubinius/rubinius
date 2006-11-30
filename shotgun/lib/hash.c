#include "shotgun.h"
#include "tuple.h"
#include "hash.h"

#define Increments 16

OBJECT hash_new(STATE) {
  OBJECT hsh;
  hsh = hash_allocate(state);
  hash_setup(state, hsh);
  return hsh;
}

void hash_setup(STATE, OBJECT hsh) {
  hash_set_keys(hsh, tuple_new(state, Increments));
  hash_set_values(hsh, tuple_new(state, Increments));
  hash_set_bins(hsh, I2N(Increments));
  hash_set_entries(hsh, I2N(0));
  hash_set_default(hsh, Qnil);
}

static OBJECT entry_new(STATE, unsigned int hsh, OBJECT key, OBJECT data) {
  OBJECT tup = tuple_new(state, 4);
  tuple_put(state, tup, 0, I2N(hsh));
  tuple_put(state, tup, 1, key);
  tuple_put(state, tup, 2, data);
  tuple_put(state, tup, 3, Qnil);
  return tup;
}

static OBJECT entry_append(STATE, OBJECT top, OBJECT nxt) {
  OBJECT cur, last;
  int depth = 1;
  cur = tuple_at(state, top, 3);
  last = top;
  while(!NIL_P(cur)) {
    last = cur;
    cur = tuple_at(state, cur, 3);
    depth++;
  }
  
  // printf("Inserted new key to hash %d levels deep.\n", depth);
  tuple_put(state, last, 3, nxt);
  return nxt;
}

static OBJECT add_entry(STATE, OBJECT h, unsigned int hsh, OBJECT ent) {
  unsigned int bin;
  OBJECT entry;
  
  bin = hsh % FIXNUM_TO_INT(hash_get_bins(h));
  entry = tuple_at(state, hash_get_values(h), bin);
  
  if(NIL_P(entry)) {
    tuple_put(state, hash_get_values(h), bin, ent);
  } else {
    entry_append(state, entry, ent);
  }
  
  hash_set_entries(h, I2N(FIXNUM_TO_INT(hash_get_entries(h)) + 1));
  return ent;
}

static OBJECT find_entry(STATE, OBJECT h, unsigned int hsh) {
  unsigned int bin, bins;
  OBJECT entry, th;
  
  bins = (unsigned int)FIXNUM_TO_INT(hash_get_bins(h));
  bin = hsh % bins;
  entry = tuple_at(state, hash_get_values(h), bin);
  
  // printf("start: %x, %ud, %d, %d\n", entry, hsh, bin, FIXNUM_TO_INT(hash_get_bins(h)));
  
  while(!NIL_P(entry)) {
    // printf("entry: %x\n", entry);
    th = tuple_at(state, entry, 0);
    if(FIXNUM_TO_INT(th) == hsh) {
      return entry;
    }
    entry = tuple_at(state, entry, 3);
  }
  return Qnil;
}

OBJECT hash_add(STATE, OBJECT h, unsigned int hsh, OBJECT key, OBJECT data) {
  OBJECT entry, keys;
  int i;
  
  // printf("hash_add: adding %od\n",hsh);
  entry = find_entry(state, h, hsh);
  
  if(RTEST(entry)) {
    tuple_put(state, entry, 2, data);
    return data;
  }
  
  entry = entry_new(state, hsh, key, data);
  add_entry(state, h, hsh, entry);
  i = FIXNUM_TO_INT(hash_get_entries(h));
  keys = hash_get_keys(h);
  if(i == NUM_FIELDS(keys)) {
    keys = tuple_enlarge(state, keys, 10);
    hash_set_keys(h, keys);
  }
  tuple_put(state, keys, i-1, key);
  return data;
}

OBJECT hash_set(STATE, OBJECT hash, OBJECT key, OBJECT val) {
  return hash_add(state, hash, object_hash_int(state, key), key, val);
}

OBJECT hash_get(STATE, OBJECT hash, unsigned int hsh) {
  OBJECT entry;
  
  entry = find_entry(state, hash, hsh);
  if(RTEST(entry)) {
    return tuple_at(state, entry, 2);
  }
  
  return Qnil;
}

OBJECT hash_find(STATE, OBJECT hash, OBJECT key) {
  return hash_get(state, hash, object_hash_int(state, key));
}

OBJECT hash_delete(STATE, OBJECT self, unsigned int hsh) {
  unsigned int bin;
  OBJECT entry, th, lk, val;
  
  bin = hsh % FIXNUM_TO_INT(hash_get_bins(self));
  entry = tuple_at(state, hash_get_values(self), bin);
  
  while(!NIL_P(entry)) {
    th = tuple_at(state, entry, 0);
    lk = tuple_at(state, entry, 3);
    
    if(FIXNUM_TO_INT(th) == hsh) {
      val = tuple_at(state, entry, 2);
      tuple_put(state, hash_get_values(self), bin, lk);
      hash_set_entries(self, I2N(FIXNUM_TO_INT(hash_get_entries(self)) - 1));
      return val;
    }
    
    entry = lk;
  }
  
  return Qnil;
}

OBJECT hash_s_from_tuple(STATE, OBJECT tup) {
  OBJECT hsh, key, val;
  int i, fel;
  hsh = hash_new(state);
  
  fel = tuple_fields(state, tup);
  i = 0;
  while(i < fel) {
    key = tuple_at(state, tup, i);
    val = tuple_at(state, tup, i + 1);
    i += 2;
    hash_set(state, hsh, key, val);
  }
  
  return hsh;
}
