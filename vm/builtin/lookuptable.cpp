#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/lookuptable.hpp"
#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"

#include <iostream>

#define LOOKUPTABLE_MAX_DENSITY 0.75
#define LOOKUPTABLE_MIN_DENSITY 0.3

#define key_hash(obj) (((unsigned int)(uintptr_t)obj))
#define find_bin(hash, bins) (hash & ((bins) - 1))
#define max_density_p(ents,bins) (ents >= LOOKUPTABLE_MAX_DENSITY * bins)
#define min_density_p(ents,bins) (ents < LOOKUPTABLE_MIN_DENSITY * bins)
#define key_to_sym(key) \
  if(String* _str = try_as<String>(key)) { \
    key = _str->to_sym(state); \
  } \


namespace rubinius {

  LookupTable* LookupTable::create(STATE, size_t size) {
    LookupTable *tbl;

    tbl = (LookupTable*)state->om->new_object(G(lookuptable),
                                              LookupTable::fields);
    tbl->setup(state, size);

    return tbl;
  }

  void LookupTable::setup(STATE, size_t sz = 0) {
    if(!sz) sz = LOOKUPTABLE_MIN_SIZE;
    values(state, Tuple::create(state, sz));
    bins(state, Fixnum::from(sz));
    entries(state, Fixnum::from(0));
  }

  /* The LookupTable.allocate primitive. */
  LookupTable* LookupTable::allocate(STATE, Object* self) {
    LookupTable* tbl = create(state, LOOKUPTABLE_MIN_SIZE);
    tbl->klass(state, as<Class>(self));
    return tbl;
  }

  LookupTable* LookupTable::dup(STATE) {
    size_t size, i;
    LookupTable *dup;

    size = bins_->to_native();
    dup = LookupTable::create(state, size);
    state->om->set_class(dup, class_object(state));
    size_t num = entries_->to_native();

    Array* entries = all_entries(state);
    for(i = 0; i < num; i++) {
      Tuple* entry = as<Tuple>(entries->get(state, i));
      Object* key =   entry->at(state, 0);
      Object* value = entry->at(state, 1);
      dup->store(state, key, value);
    }
    return dup;

  }

  Object* LookupTable::entry_new(STATE, Object* key, Object* data) {
    Tuple* tup = Tuple::create(state, 3);
    tup->put(state, 0, key);
    tup->put(state, 1, data);
    tup->put(state, 2, Qnil);
    return tup;
  }

  Object* LookupTable::entry_append(STATE, Tuple* top, Object* nxt) {
    Tuple* cur = try_as<Tuple>(top->at(state, 2));
    Tuple* last = top;

    while(cur) {
      last = cur;
      cur = try_as<Tuple>(cur->at(state, 2));
    }

    last->put(state, 2, nxt);
    return nxt;
  }

  void LookupTable::redistribute(STATE, size_t size) {
    size_t num = bins_->to_native();
    Tuple* new_values = Tuple::create(state, size);

    for(size_t i = 0; i < num; i++) {
      Tuple* entry = try_as<Tuple>(values_->at(state, i));

      while(entry) {
        Tuple* link = try_as<Tuple>(entry->at(state, 2));
        entry->put(state, 2, Qnil);

        size_t bin = find_bin(key_hash(entry->at(state, 0)), size);
        Tuple* slot = try_as<Tuple>(new_values->at(state, bin));

        if(!slot) {
          new_values->put(state, bin, entry);
        } else {
          entry_append(state, slot, entry);
        }

        entry = link;
      }
    }

    values(state, new_values);
    bins(state, Fixnum::from(size));
  }

  Object* LookupTable::store(STATE, Object* key, Object* val) {
    unsigned int num_entries, num_bins, bin;
    Object* new_ent;
    Tuple* cur;
    Tuple* entry;

    key_to_sym(key);
    num_entries = entries_->to_native();
    num_bins = bins_->to_native();

    if(max_density_p(num_entries, num_bins)) {
      redistribute(state, num_bins <<= 1);
    }

    bin = find_bin(key_hash(key), num_bins);
    cur = entry = try_as<Tuple>(values_->at(state, bin));

    while(entry) {
      if(entry->at(state, 0) == key) {
        entry->put(state, 1, val);
        return val;
      }
      cur = entry;
      entry = try_as<Tuple>(entry->at(state, 2));
    }

    new_ent = entry_new(state, key, val);
    if(cur) {
      cur->put(state, 2, new_ent);
    } else {
      values_->put(state, bin, new_ent);
    }

    entries(state, Fixnum::from(num_entries + 1));
    return val;
  }

  Tuple* LookupTable::find_entry(STATE, Object* key) {
    unsigned int bin;
    Tuple* entry;

    key_to_sym(key);
    bin = find_bin(key_hash(key), bins_->to_native());

    /* HACK: This should be fixed by not storing NULLs */
    Object* data = values_->at(state, bin);

    if (!data) return NULL;

    entry = try_as<Tuple>(data);

    while(entry) {
      if(entry->at(state, 0) == key) {
        return entry;
      }
      entry = try_as<Tuple>(entry->at(state, 2));
    }
    return NULL;
  }

  /** Same as fetch(state, key). */
  Object* LookupTable::aref(STATE, Object* key) {
    Tuple* entry = find_entry(state, key);
    if(entry) return entry->at(state, 1);
    return Qnil;
  }

  /** Same as aref(state, key). */
  Object* LookupTable::fetch(STATE, Object* key) {
    Tuple* entry = find_entry(state, key);
    if(entry) return entry->at(state, 1);
    return Qnil;
  }

  Object* LookupTable::fetch(STATE, Object* key, Object* return_on_failure) {
    Tuple* entry = find_entry(state, key);

    if(entry) {
      return entry->at(state, 1);
    }

    return return_on_failure;
  }

  Object* LookupTable::fetch(STATE, Object* key, bool* found) {
    Tuple* entry = find_entry(state, key);
    if(entry) {
      *found = true;
      return entry->at(state, 1);
    }

    *found = false;
    return Qnil;
  }

  /* lookuptable_find returns Qundef if there is not entry
   * referenced by 'key' in the LookupTable. This is useful
   * to distinguish x = {} from x = {:a => nil} and is used
   * in cpu.c in e.g. cpu_const_get_in_context.
   */
  Object* LookupTable::find(STATE, Object* key) {
    Tuple* entry = find_entry(state, key);
    if(entry) {
      return entry->at(state, 1);
    }
    return Qundef;
  }

  Object* LookupTable::remove(STATE, Object* key) {
    hashval bin;
    Object* val;
    Tuple* entry;
    Tuple* lst;

    key_to_sym(key);

    size_t num_entries = entries_->to_native();
    size_t num_bins = bins_->to_native();

    if(min_density_p(num_entries, num_bins) && (num_bins >> 1) >= LOOKUPTABLE_MIN_SIZE) {
      redistribute(state, num_bins >>= 1);
    }

    bin = find_bin(key_hash(key), num_bins);
    entry = try_as<Tuple>(values_->at(state, bin));

    lst = NULL;

    while(entry) {
      Object* link = entry->at(state, 2);

      if(entry->at(state, 0) == key) {
        val = entry->at(state, 1);
        if(lst) {
          lst->put(state, 2, link);
        } else {
          values_->put(state, bin, link);
        }
        entries(state, Fixnum::from(entries_->to_native() - 1));
        return val;
      }

      lst = entry;
      entry = try_as<Tuple>(link);
    }

    return Qnil;
  }

  Object* LookupTable::has_key(STATE, Object* key) {
    Tuple* entry = find_entry(state, key);

    if(entry) return Qtrue;
    return Qfalse;
  }

  Array* LookupTable::collect(STATE, LookupTable* tbl, Object* (*action)(STATE, Tuple*)) {
    size_t i, j;
    Tuple* values;
    Tuple* entry;

    Array* ary = Array::create(state, tbl->entries()->to_native());
    size_t num_bins = tbl->bins()->to_native();
    values = tbl->values();

    for(i = j = 0; i < num_bins; i++) {
      entry = try_as<Tuple>(values->at(state, i));

      while(entry) {
        ary->set(state, j++, action(state, entry));
        entry = try_as<Tuple>(entry->at(state, 2));
      }
    }
    return ary;
  }

  Object* LookupTable::get_key(STATE, Tuple* entry) {
    return entry->at(state, 0);
  }

  Array* LookupTable::all_keys(STATE) {
    return collect(state, this, get_key);
  }

  Object* LookupTable::get_value(STATE, Tuple* entry) {
    return entry->at(state, 1);
  }

  Array* LookupTable::all_values(STATE) {
    return collect(state, this, get_value);
  }

  Object* LookupTable::get_entry(STATE, Tuple* entry) {
    return entry;
  }

  Array* LookupTable::all_entries(STATE) {
    return collect(state, this, get_entry);
  }

  void LookupTable::Info::show(STATE, Object* self, int level) {
    LookupTable* tbl = as<LookupTable>(self);
    size_t size = tbl->entries()->to_native();
    Array* keys = tbl->all_keys(state);

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    indent(++level);
    for(size_t i = 0; i < size; i++) {
      std::cout << ":" << as<Symbol>(keys->get(state, i))->c_str(state);
      if(i < size - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    close_body(level);
  }
}
