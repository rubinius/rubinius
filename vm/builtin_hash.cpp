#include "builtin.hpp"

#define find_bin(hash, total) (hash & (total - 1))

#define CSM_SIZE 12
namespace rubinius {
  Hash* Hash::create(STATE, size_t size) {
    Hash *tbl;

    tbl = (Hash*)state->om->new_object(G(hash), Hash::fields);
    tbl->setup(state, size);

    return tbl;
  }

  void Hash::setup(STATE, size_t size) {
    keys = Qnil;
    SET(this, values, Tuple::create(state, size));
    SET(this, bins, Object::i2n(size));
    SET(this, entries, Object::i2n(0));
    default_value = Qnil;
  }

  Hash* Hash::dup(STATE) {
    size_t size, i;
    Hash *dup;
    OBJECT tmp;
    Tuple *vals, *lst, *ent, *next;

    size = bins->n2i();
    dup = Hash::create(state, size);
    state->om->set_class(dup, class_object(state));

    SET(dup, bins, Object::i2n(size));
    SET(dup, entries, entries);
    SET(dup, default_value, dup->default_value);
    dup->keys = Qnil;

    vals = Tuple::create(state, size);
    SET(dup, values, vals);

    for(i = 0; i < size; i++) {
      tmp = try_as<Tuple>(values->at(i));
      if(!tmp) continue;

      ent = as<Tuple>(tmp->dup(state));
      vals->put(state, i, ent);
      next = try_as<Tuple>(ent->at(3));
      lst = ent;

      while(next) {
        next = as<Tuple>(next->dup(state));
        lst->put(state, 3, next);
        lst = next;
        next = try_as<Tuple>(next->at(3));
      }
    }

    return dup;
  }

  Tuple* Hash::entry_new(STATE, hashval hsh, OBJECT key, OBJECT data) {
    Tuple* tup = Tuple::create(state, 4);
    tup->put(state, 0, Object::i2n(hsh));
    tup->put(state, 1, key);
    tup->put(state, 2, data);
    tup->put(state, 3, Qnil);
    return tup;
  }

  Tuple* Hash::entry_append(STATE, Tuple* top, Tuple* nxt) {
    int depth = 1;
    Tuple* cur = try_as<Tuple>(top->at(3));
    Tuple* last = top;

    while(cur) {
      last = cur;
      cur = try_as<Tuple>(cur->at(3));
      depth++;
    }

    last->put(state, 3, nxt);
    return nxt;
  }

  OBJECT Hash::add_entry(STATE, hashval hsh, Tuple* ent) {
    size_t bin = find_bin(hsh, bins->n2i());

    if(Tuple* entry = try_as<Tuple>(values->at(bin))) {
      entry_append(state, entry, ent);
    } else {
      values->put(state, bin, ent);
    }

    entries = Object::i2n(entries->n2i() + 1);
    return ent;
  }

  void Hash::redistribute(STATE) {
    size_t num = bins->n2i();

    /* TODO: calculate the size both up and down.
     * i.e. don't assume we're only growing
     */
    size_t size = num * 2;
    Tuple* new_values = Tuple::create(state, size);

    for(size_t i = 0; i < num; i++) {
      Tuple* entry = try_as<Tuple>(values->at(i));

      while(entry) {
        Tuple* next = try_as<Tuple>(entry->at(3));
        entry->put(state, 3, Qnil);

        native_int hash = as<Integer>(entry->at(0))->n2i();
        size_t bin = find_bin(hash, size);
        if(Tuple* slot = try_as<Tuple>(new_values->at(bin))) {
          entry_append(state, slot, entry);
        } else {
          new_values->put(state, bin, entry);
        }

        entry = next;
      }
    }

    SET(this, values, new_values);
    bins = Object::i2n(size);
  }

  /* Find the entry that has a hash value of +hsh+. Return NULL if nothing
   * found. */
  Tuple* Hash::find_entry(STATE, hashval hsh) {
    size_t num = bins->n2i();
    size_t bin = find_bin(hsh, num);
    Tuple* entry = try_as<Tuple>(values->at(bin));
    if(!entry) return NULL;

    for(;;) {
      INTEGER th = as<Integer>(entry->at(0));
      if((hashval)th->n2i() == hsh) {
        return entry;
      }

      if((entry = try_as<Tuple>(entry->at(3))) == NULL) return NULL;
    }
    return NULL;
  }

  OBJECT Hash::add(STATE, hashval hsh, OBJECT key, OBJECT data) {
    Tuple* entry = find_entry(state, hsh);

    if(entry) {
      entry->put(state, 2, data);
      return data;
    }

    if(hash_redistribute_p(this)) redistribute(state);

    entry = (Tuple*)entry_new(state, hsh, key, data);
    add_entry(state, hsh, entry);
    return data;
  }

  OBJECT Hash::set(STATE, OBJECT key, OBJECT val) {
    return add(state, key->hash(state), key, val);
  }

  OBJECT Hash::get(STATE, hashval hsh) {
    Tuple* entry = find_entry(state, hsh);
    if(entry) {
      return entry->at(2);
    }

    return Qnil;
  }

  /* Find the +value+ for +key+, having hash value +hash+. 
   * Uses +==+ to verify the key in the table matches +key+.
   * Return TRUE if key was found, and *value will be filled. */
  int Hash::lookup(STATE, OBJECT key, hashval hash, OBJECT *value) {
    OBJECT hk;

    Tuple* ent = find_entry(state, hash);
    if(!ent) return FALSE;

    while((hashval)as<Integer>(ent->at(0))->n2i() == hash) {
      hk = ent->at(1);
      if(hk == key) {
        *value = ent->at(2);
        return TRUE;
      }

      ent = try_as<Tuple>(ent->at(3));
      if(!ent) break;
    }

    return FALSE;
  }

  /* Find the +value+ for +key+, having hash value +hash+. 
   * Uses +compare+ to verify the key in the table matches +key+.
   * Return TRUE if key was found, and *value will be filled. */
  int Hash::lookup2(STATE, int (*compare)(STATE, OBJECT, OBJECT),
      OBJECT key, hashval hash, OBJECT *value) {
    OBJECT hk;

    Tuple* ent = find_entry(state, hash);
    if(!ent) return FALSE;

    while((hashval)as<Integer>(ent->at(0))->n2i() == hash) {
      hk = ent->at(1);
      if(compare(state, hk, key)) {
        *value = ent->at(2);
        return TRUE;
      }

      ent = try_as<Tuple>(ent->at(3));
      if(!ent) break;
    }

    return FALSE;
  }

  void Hash::assign(STATE, int (*compare)(STATE, OBJECT, OBJECT),
      OBJECT key, hashval hash, OBJECT value) {
    OBJECT hk;
    Tuple* ent;
    Tuple* base;

    base = ent = find_entry(state, hash);
    if(ent) {
      while((hashval)as<Integer>(ent->at(0))->n2i() == hash) {
        hk = ent->at(1);
        if(compare(state, hk, key)) {
          ent->put(state, 2, value);
          return;
        }

        ent = try_as<Tuple>(ent->at(3));
        if(!ent) break;
      }

    }

    if(hash_redistribute_p(this)) {
      redistribute(state);
    }

    if(base) {
      ent = entry_new(state, hash, key, value);
      entry_append(state, base, ent);
      entries = Object::i2n(entries->n2i() + 1);
      return;
    }

    ent = entry_new(state, hash, key, value);
    add_entry(state, hash, ent);
  }

  /* This version of hash_get returns Qundef if the entry was not found.
   * This is handy when you need to tell the difference between:
   * x = {} and x = {:a => nil}
   */

  OBJECT Hash::get_undef(STATE, hashval hsh) {
    Tuple* entry = find_entry(state, hsh);
    if(entry) {
      return entry->at(2);
    } else {
      return Qundef;
    }
  }

  OBJECT Hash::remove(STATE, hashval hsh) {
    hashval bin;
    OBJECT val;
    Tuple* entry;
    Tuple* lst;
    Tuple *lk;
    INTEGER th;

    bin = find_bin(hsh, bins->n2i());
    entry = (Tuple*)values->at(bin);

    lst = (Tuple*)Qnil;

    while(!entry->nil_p()) {
      th = as<Integer>(entry->at(0));
      lk = (Tuple*)entry->at(3);

      if((hashval)th->n2i() == hsh) {
        val = entry->at(2);
        if(lst->nil_p()) {
          values->put(state, bin, lk);
        } else {
          lst->put(state, 3, lk);
        }
        entries = Object::i2n(entries->n2i() - 1);
        return val;
      }

      lst = entry;
      entry = lk;
    }

    return Qnil;
  }

  Hash* Hash::from_tuple(STATE, Tuple* tup) {
    OBJECT key, val;
    int i, fel;

    Hash* hsh = (Hash*)Hash::create(state);

    fel = tup->field_count;
    i = 0;
    while(i < fel) {
      key = tup->at(i);
      val = tup->at(i + 1);
      i += 2;
      hsh->set(state, key, val);
    }

    return hsh;
  }

  Tuple* Hash::csm_new(STATE) {
    return Tuple::create(state, CSM_SIZE);
  }

  OBJECT Hash::csm_find(STATE, Tuple* csm, OBJECT key) {
    OBJECT t, val;
    int i;

    for(i = 0; i < CSM_SIZE; i += 2) {
      t = csm->at(i);

      if(t == key) {
        val = csm->at(i + 1);
        return val;
      }
    }

    return Qnil;
  }

  OBJECT Hash::csm_add(STATE, Tuple* csm, OBJECT key, OBJECT val) {
    int i;
    OBJECT tmp;

    for(i = 0; i < CSM_SIZE; i += 2) {
      tmp = csm->at(i);
      if(tmp == key || tmp->nil_p()) {
        csm->put(state, i, key);
        csm->put(state, i + 1, val);
        return Qtrue;
      }
    }

    return Qfalse;
  }

  Hash* Hash::csm_into_hash(STATE, Tuple* csm) {
    int i;
    OBJECT k;

    Hash* hsh = (Hash*)Hash::create(state);

    for(i = 0; i < CSM_SIZE; i += 2) {
      k = csm->at(i);
      if(!k->nil_p()) {
        hsh->set(state, k, csm->at(i + 1));
      }
    }

    return hsh;
  }

  LookupTable* Hash::csm_into_lookuptable(STATE, Tuple* csm) {
    int i;
    OBJECT k;

    LookupTable* tbl = (LookupTable*)LookupTable::create(state);

    for(i = 0; i < CSM_SIZE; i += 2) {
      k = csm->at(i);
      if(!k->nil_p()) {
        tbl->store(state, k, csm->at(i + 1));
      }
    }

    return tbl;
  }

};
