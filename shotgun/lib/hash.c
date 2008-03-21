#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/lookuptable.h"

/* MINSIZE MUST be a power of 2 */
#define MINSIZE 16

#define Increments 16

#define find_bin(hash, total) (hash & (total - 1))

OBJECT hash_new(STATE) {
  OBJECT hsh;
  hsh = hash_allocate(state);
  hash_setup(state, hsh, MINSIZE);
  return hsh;
}

/* size MUST be a power of 2 */
OBJECT hash_new_sized(STATE, int size) {
  OBJECT hsh;
  hsh = hash_allocate(state);
  hash_setup(state, hsh, size);
  return hsh;
}

void hash_setup(STATE, OBJECT hsh, int size) {
  int sz;
  sz = size == 0 ? MINSIZE : size;
  // TODO: Remove @keys field
  hash_set_keys(hsh, Qnil);
  hash_set_values(hsh, tuple_new(state, sz));
  hash_set_bins(hsh, I2N(sz));
  hash_set_entries(hsh, I2N(0));
  hash_set_default(hsh, Qnil);
}

OBJECT hash_dup(STATE, OBJECT hsh) {
  int sz, i;
  OBJECT dup, cv, vals, ent, next, lst, tmp;

  sz = N2I(hash_get_bins(hsh));
  dup = hash_new_sized(state, sz);
  SET_CLASS(dup, object_class(state, hsh));

  hash_set_bins(dup, I2N(sz));
  hash_set_entries(dup, hash_get_entries(hsh));
  hash_set_default(dup, hash_get_default(hsh));
  // TODO: Remove @keys field
  hash_set_keys(dup, Qnil);

  cv = hash_get_values(hsh);

  vals = tuple_new(state, sz);
  hash_set_values(dup, vals);

  for(i = 0; i < sz; i++) {
    tmp = tuple_at(state, cv, i);
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
  unsigned int bin, bins;
  OBJECT entry;


  bins = (unsigned int)N2I(hash_get_bins(h));
  bin = find_bin(hsh, bins);

  entry = tuple_at(state, hash_get_values(h), bin);

  if(NIL_P(entry)) {
    tuple_put(state, hash_get_values(h), bin, ent);
  } else {
    entry_append(state, entry, ent);
  }

  hash_set_entries(h, I2N(N2I(hash_get_entries(h)) + 1));
  return ent;
}

/* TODO: Why do we use signed int for bins and then use a
 * loop like for(i = 0; i < bins; i++) ?
 */
void hash_redistribute(STATE, OBJECT hsh) {
  int i, bins, size;
  unsigned int bin, hash;
  OBJECT values, new_values, entry, next, slot;

  bins = N2I(hash_get_bins(hsh));
  values = hash_get_values(hsh);

  /* TODO: calculate the size both up and down.
   * i.e. don't assume we're only growing
   */
  size = bins * 2;
  new_values = tuple_new(state, size);

  // printf("Hash redistribute %p, %d => %d (%d)\n", hsh, bins, size, N2I(hash_get_entries(hsh)));

  for(i = 0; i < bins; i++) {
    entry = tuple_at(state, values, i);

    while(!NIL_P(entry)) {
      next = tuple_at(state, entry, 3);
      tuple_put(state, entry, 3, Qnil);

      hash = N2I(tuple_at(state, entry, 0));
      bin = find_bin(hash, size);
      slot = tuple_at(state, new_values, bin);

      if(NIL_P(slot)) {
        tuple_put(state, new_values, bin, entry);
      } else {
        entry_append(state, slot, entry);
      }

      entry = next;
    }
  }

  hash_set_values(hsh, new_values);
  hash_set_bins(hsh, I2N(size));
}

OBJECT hash_find_entry(STATE, OBJECT h, unsigned int hsh) {
  unsigned int bin, bins;
  OBJECT entry, th;

  bins = (unsigned int)N2I(hash_get_bins(h));
  bin = find_bin(hsh, bins);
  entry = tuple_at(state, hash_get_values(h), bin);

  // printf("start: %x, %ud, %d, %d\n", entry, hsh, bin, N2I(hash_get_bins(h)));

  while(!NIL_P(entry)) {
    // printf("entry: %x\n", entry);
    th = tuple_at(state, entry, 0);
    if(N2I(th) == hsh) {
      return entry;
    }
    entry = tuple_at(state, entry, 3);
  }
  return Qnil;
}

OBJECT hash_add(STATE, OBJECT h, unsigned int hsh, OBJECT key, OBJECT data) {
  OBJECT entry;
  int i;

  // printf("hash_add: adding %od\n",hsh);
  entry = hash_find_entry(state, h, hsh);

  if(RTEST(entry)) {
    tuple_put(state, entry, 2, data);
    return data;
  }

  if(hash_redistribute_p(h)) {
    hash_redistribute(state, h);
  }

  i = N2I(hash_get_entries(h));
  entry = entry_new(state, hsh, key, data);
  add_entry(state, h, hsh, entry);
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

/* Find the +value+ for +key+, having hash value +hash+.
 * Uses +==+ to verify the key in the table matches +key+.
 * Return TRUE if key was found, and *value will be filled. */
int hash_lookup(STATE, OBJECT tbl, OBJECT key, unsigned int hash, OBJECT *value) {
  OBJECT ent, hk;

  ent = hash_find_entry(state, tbl, hash);
  if(REFERENCE_P(ent)) {
    while(N2I(tuple_at(state, ent, 0)) == hash) {
      hk = tuple_at(state, ent, 1);
      if(hk == key) {
        *value = tuple_at(state, ent, 2);
        return TRUE;
      }

      ent = tuple_at(state, ent, 3);
      if(NIL_P(ent)) break;
    }
  }

  return FALSE;
}

/* Find the +value+ for +key+, having hash value +hash+.
 * Uses +compare+ to verify the key in the table matches +key+.
 * Return TRUE if key was found, and *value will be filled. */
int hash_lookup2(STATE, int (*compare)(STATE, OBJECT, OBJECT),
    OBJECT tbl, OBJECT key, unsigned int hash, OBJECT *value) {
  OBJECT ent, hk;

  ent = hash_find_entry(state, tbl, hash);
  if(REFERENCE_P(ent)) {
    while(N2I(tuple_at(state, ent, 0)) == hash) {
      hk = tuple_at(state, ent, 1);
      if(compare(state, hk, key)) {
        *value = tuple_at(state, ent, 2);
        return TRUE;
      }

      ent = tuple_at(state, ent, 3);
      if(NIL_P(ent)) break;
    }
  }

  return FALSE;
}

void hash_assign(STATE, int (*compare)(STATE, OBJECT, OBJECT), OBJECT tbl,
    OBJECT key, unsigned int hash, OBJECT value) {
  OBJECT base, ent, hk;

  base = ent = hash_find_entry(state, tbl, hash);
  if(REFERENCE_P(ent)) {
    while(N2I(tuple_at(state, ent, 0)) == hash) {
      hk = tuple_at(state, ent, 1);
      if(compare(state, hk, key)) {
        tuple_put(state, ent, 2, value);
        return;
      }

      ent = tuple_at(state, ent, 3);
      if(NIL_P(ent)) break;
    }

  }

  if(hash_redistribute_p(tbl)) {
    hash_redistribute(state, tbl);
  }

  if(REFERENCE_P(base)) {
    ent = entry_new(state, hash, key, value);
    entry_append(state, base, ent);
    hash_set_entries(tbl, I2N(N2I(hash_get_entries(tbl)) + 1));
    return;
  }

  ent = entry_new(state, hash, key, value);
  add_entry(state, tbl, hash, ent);
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

  bin = find_bin(hsh, N2I(hash_get_bins(self)));
  entry = tuple_at(state, hash_get_values(self), bin);

  lst = Qnil;

  while(!NIL_P(entry)) {
    th = tuple_at(state, entry, 0);
    lk = tuple_at(state, entry, 3);

    if(N2I(th) == hsh) {
      val = tuple_at(state, entry, 2);
      if(NIL_P(lst)) {
        tuple_put(state, hash_get_values(self), bin, lk);
      } else {
        tuple_put(state, lst, 3, lk);
      }
      hash_set_entries(self, I2N(N2I(hash_get_entries(self)) - 1));
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

OBJECT csm_into_lookuptable(STATE, OBJECT csm) {
  int i;
  OBJECT k, tbl;

  tbl = lookuptable_new(state);

  for(i = 0; i < CSM_SIZE; i += 2) {
    k = tuple_at(state, csm, i);
    if(!NIL_P(k)) {
      lookuptable_store(state, tbl, k, tuple_at(state, csm, i + 1));
    }
  }

  return tbl;
}
