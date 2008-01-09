#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/hash.h"

/* Adapted from st.c in 1.8.5 */

/*
Table of prime numbers 2^n+a, 2<=n<=30.
*/
static const long primes[] = {
        8 + 3,
        16 + 3,
        23,
        32 + 5,
        43,
        53,
        59,
        64 + 3,
        73,
        83,
        97,
        109,
        128 + 3,
        137,
        149,
        157,
        167,
        179,
        191,
        197,
        211,
        229,
        256 + 27,
        512 + 9,
        1024 + 9,
        2048 + 5,
        4096 + 3,
        8192 + 27,
        16384 + 43,
        32768 + 3,
        65536 + 45,
        131072 + 29,
        262144 + 3,
        524288 + 21,
        1048576 + 7,
        2097152 + 17,
        4194304 + 15,
        8388608 + 9,
        16777216 + 43,
        33554432 + 35,
        67108864 + 15,
        134217728 + 29,
        268435456 + 3,
        536870912 + 11,
        1073741824 + 85,
        0
};

#define MINSIZE 8

static int hash_new_size(int size) {
  int i, p;
  int newsize;

  for (i = 0, newsize = MINSIZE;
       i < sizeof(primes)/sizeof(primes[0]);
       i++)
  {
    p = primes[i];
    if(p > size) return p;
  }
  /* Ran out of polynomials */
  return -1;                  /* should raise exception */
}

#define MAX_DENSITY 5

/* end adaptation. */

#define Increments 16

OBJECT hash_new(STATE) {
  OBJECT hsh;
  hsh = hash_allocate(state);
  hash_setup(state, hsh, 0);
  return hsh;
}

OBJECT hash_new_sized(STATE, int size) {
  OBJECT hsh;
  hsh = hash_allocate(state);
  hash_setup(state, hsh, size);
  return hsh;
}

void hash_setup(STATE, OBJECT hsh, int size) {
  int sz;
  sz = hash_new_size(size);
  hash_set_keys(hsh, tuple_new(state, sz));
  hash_set_values(hsh, tuple_new(state, sz));
  hash_set_bins(hsh, I2N(sz));
  hash_set_entries(hsh, I2N(0));
  hash_set_default(hsh, Qnil);
}

OBJECT hash_dup(STATE, OBJECT hsh) {
  int sz, i;
  OBJECT dup, vals, ent, next, lst, tmp;
  
  sz = FIXNUM_TO_INT(hash_get_bins(hsh));
  dup = hash_new_sized(state, sz);
  dup->klass = object_class(state, hsh);
  
  hash_set_bins(dup, I2N(sz));
  hash_set_entries(dup, hash_get_entries(hsh));
  hash_set_default(dup, hash_get_default(hsh));
  hash_set_keys(dup, tuple_dup(state, hash_get_keys(hsh)));
  
  vals = tuple_new(state, sz);
  
  for(i = 0; i < sz; i++) {
    tmp = tuple_at(state, hsh, i);
    if(NIL_P(tmp)) continue;
    
    ent = tuple_dup(state, tmp);
    tuple_put(state, vals, i, ent);
    
    next = tuple_at(state, ent, 3);
    lst = ent;
    while(!NIL_P(next)) {
      next = tuple_dup(state, next);
      tuple_put(state, lst, 3, next);
      lst = next;
      next = tuple_at(state, next, 3);
    }    
  }
  
  return dup;
}

static void hash_rehash(STATE, OBJECT hsh, int _ents) {
  int new_bins, i, old_bins;
  unsigned int bin, hv;
  OBJECT tbl, tup, ent, next;
  
  old_bins = FIXNUM_TO_INT(hash_get_bins(hsh));
  new_bins = hash_new_size(old_bins + 1);
  tup = tuple_new(state, new_bins);
  tbl = hash_get_values(hsh);
  
  // printf("Rehash %p, %d => %d (%d)\n", hsh, old_bins, new_bins, FIXNUM_TO_INT(hash_get_entries(hsh)));
  
  for(i = 0; i < old_bins; i++) {
    ent = tuple_at(state, tbl, i);
    while(!NIL_P(ent)) {
      next = tuple_at(state, ent, 3);
      hv = (unsigned int)FIXNUM_TO_INT(tuple_at(state, ent, 0));
      
      bin = hv % new_bins;
      tuple_put(state, ent, 3, tuple_at(state, tup, (int)bin));
      tuple_put(state, tup, (int)bin, ent);
      
      ent = next;
    }
  }
  
  hash_set_values(hsh, tup);
  hash_set_bins(hsh, I2N(new_bins));
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

OBJECT hash_find_entry(STATE, OBJECT h, unsigned int hsh) {
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
  int i, b;
  
  // printf("hash_add: adding %od\n",hsh);
  entry = hash_find_entry(state, h, hsh);
  
  if(RTEST(entry)) {
    tuple_put(state, entry, 2, data);
    return data;
  }
  
  i = FIXNUM_TO_INT(hash_get_entries(h));
  b = FIXNUM_TO_INT(hash_get_bins(h));
  
  if(i / b > MAX_DENSITY) {
    hash_rehash(state, h, i);
  }
  
  entry = entry_new(state, hsh, key, data);
  add_entry(state, h, hsh, entry);
  keys = hash_get_keys(h);
  if(i == NUM_FIELDS(keys)) {
    keys = tuple_enlarge(state, keys, 10);
    hash_set_keys(h, keys);
  }
  //tuple_put(state, keys, i-1, key);
  return data;
}

OBJECT hash_set(STATE, OBJECT hash, OBJECT key, OBJECT val) {
  return hash_add(state, hash, object_hash_int(state, key), key, val);
}

OBJECT hash_get(STATE, OBJECT hash, unsigned int hsh) {
  OBJECT entry;
  
  entry = hash_find_entry(state, hash, hsh);
  if(RTEST(entry)) {
    return tuple_at(state, entry, 2);
  }
  
  return Qnil;
}

/* This version of hash_get returns Qundef if the entry was not found.
 * This is handy when you need to tell the difference between:
 * x = {} and x = {:a => nil}
 */

OBJECT hash_get_undef(STATE, OBJECT hash, unsigned int hsh) {
  OBJECT entry;
  
  entry = hash_find_entry(state, hash, hsh);
  if(RTEST(entry)) {
    return tuple_at(state, entry, 2);
  } else {
    return Qundef;
  }
}

OBJECT hash_delete(STATE, OBJECT self, unsigned int hsh) {
  unsigned int bin;
  OBJECT entry, th, lk, val, lst;
  
  bin = hsh % FIXNUM_TO_INT(hash_get_bins(self));
  entry = tuple_at(state, hash_get_values(self), bin);
  
  lst = Qnil;
  
  while(!NIL_P(entry)) {
    th = tuple_at(state, entry, 0);
    lk = tuple_at(state, entry, 3);
    
    if(FIXNUM_TO_INT(th) == hsh) {
      val = tuple_at(state, entry, 2);
      if(NIL_P(lst)) {
        tuple_put(state, hash_get_values(self), bin, lk);
      } else {
        tuple_put(state, lst, 3, lk);
      }
      hash_set_entries(self, I2N(FIXNUM_TO_INT(hash_get_entries(self)) - 1));
      return val;
    }
    
    lst = entry;
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

OBJECT csm_find(STATE, OBJECT csm, OBJECT key) {
  OBJECT t, val;
  int i;
  
  for(i = 0; i < CSM_SIZE; i += 2) {
    t = tuple_at(state, csm, i);
    
    if(t == key) {
      val = tuple_at(state, csm, i + 1);
      // printf("Found %s: %s\n", _inspect(key), _inspect(val));
      return val;
    }
  }
  
  //printf("Unable to find ivar %s\n", _inspect(key));
  
  return Qnil;
}

OBJECT csm_add(STATE, OBJECT csm, OBJECT key, OBJECT val) {
  int i;
  OBJECT tmp;
  
  for(i = 0; i < CSM_SIZE; i += 2) {
    tmp = tuple_at(state, csm, i);
    if(tmp == key || NIL_P(tmp)) {
      // printf("Added ivar %s at %d\n", _inspect(key), i);
      tuple_put(state, csm, i, key);
      tuple_put(state, csm, i + 1, val);
      return Qtrue;
    }
  }
  
  return Qfalse;
}

OBJECT csm_into_hash(STATE, OBJECT csm) {
  int i;
  OBJECT k, hsh;
  
  hsh = hash_new(state);
  
  for(i = 0; i < CSM_SIZE; i += 2) {
    k = tuple_at(state, csm, i);
    if(!NIL_P(k)) {
      hash_set(state, hsh, k, tuple_at(state, csm, i + 1));
    }
  }
  
  return hsh;
}
