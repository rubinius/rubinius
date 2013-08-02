#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "object_utils.hpp"

#define LOOKUPTABLE_MAX_DENSITY 0.75
#define LOOKUPTABLE_MIN_DENSITY 0.3

#define key_hash(obj) (((unsigned int)(uintptr_t)obj) >> 2)
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

    tbl = state->new_object<LookupTable>(G(lookuptable));
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

  LookupTable* LookupTable::duplicate(STATE) {
    size_t size, i;
    LookupTable *dup;

    size = bins_->to_native();
    dup = LookupTable::create(state, size);

    // Allow for subclassing.
    dup->klass(state, class_object(state));

    size_t num = entries_->to_native();

    Array* entries = all_entries(state);
    for(i = 0; i < num; i++) {
      LookupTableBucket* entry = as<LookupTableBucket>(entries->get(state, i));
      dup->store(state, entry->key(), entry->value());
    }
    return dup;
  }

  void LookupTable::redistribute(STATE, size_t size) {
    size_t num = bins_->to_native();
    Tuple* new_values = Tuple::create(state, size);

    for(size_t i = 0; i < num; i++) {
      LookupTableBucket* entry = try_as<LookupTableBucket>(values_->at(state, i));

      while(entry) {
        LookupTableBucket* link = try_as<LookupTableBucket>(entry->next());
        entry->next(state, nil<LookupTableBucket>());

        size_t bin = find_bin(key_hash(entry->key()), size);
        LookupTableBucket* slot = try_as<LookupTableBucket>(new_values->at(state, bin));

        if(slot) {
          slot->append(state, entry);
        } else {
          new_values->put(state, bin, entry);
        }

        entry = link;
      }
    }

    values(state, new_values);
    bins(state, Fixnum::from(size));
  }

  Object* LookupTable::store(STATE, Object* key, Object* val) {
    unsigned int num_entries, num_bins, bin;
    LookupTableBucket* entry;
    LookupTableBucket* last = NULL;

    num_entries = entries_->to_native();
    num_bins = bins_->to_native();

    if(max_density_p(num_entries, num_bins)) {
      redistribute(state, num_bins <<= 1);
    }

    key_to_sym(key);
    bin = find_bin(key_hash(key), num_bins);
    entry = try_as<LookupTableBucket>(values_->at(state, bin));

    while(entry) {
      if(entry->key() == key) {
        entry->value(state, val);
        return val;
      }
      last = entry;
      entry = try_as<LookupTableBucket>(entry->next());
    }

    if(last) {
      last->next(state, LookupTableBucket::create(state, key, val));
    } else {
      values_->put(state, bin, LookupTableBucket::create(state, key, val));
    }

    entries(state, Fixnum::from(num_entries + 1));
    return val;
  }

  LookupTableBucket* LookupTable::find_entry(STATE, Object* key) {
    unsigned int bin;

    key_to_sym(key);
    bin = find_bin(key_hash(key), bins_->to_native());
    LookupTableBucket *entry = try_as<LookupTableBucket>(values_->at(state, bin));

    while(entry) {
      if(entry->key() == key) {
        return entry;
      }
      entry = try_as<LookupTableBucket>(entry->next());
    }
    return nil<LookupTableBucket>();
  }

  /** Same as fetch(state, key). */
  Object* LookupTable::aref(STATE, Object* key) {
    LookupTableBucket* entry = find_entry(state, key);
    if(entry->nil_p()) return cNil;
    return entry->value();
  }

  /** Same as aref(state, key). */
  Object* LookupTable::fetch(STATE, Object* key) {
    LookupTableBucket* entry = find_entry(state, key);
    if(entry->nil_p()) return cNil;
    return entry->value();
  }

  Object* LookupTable::fetch(STATE, Object* key, Object* return_on_failure) {
    LookupTableBucket* entry = find_entry(state, key);

    if(entry->nil_p()) {
      return return_on_failure;
    }

    return entry->value();
  }

  Object* LookupTable::fetch(STATE, Object* key, bool* found) {
    LookupTableBucket* entry = find_entry(state, key);
    if(entry->nil_p()) {
      *found = false;
      return cNil;
    }

    *found = true;
    return entry->value();
  }

  /* lookuptable_find returns cUndef if there is not entry
   * referenced by 'key' in the LookupTable. This is useful
   * to distinguish x = {} from x = {:a => nil} and is used
   * in cpu.c in e.g. cpu_const_get_in_context.
   */
  Object* LookupTable::find(STATE, Object* key) {
    LookupTableBucket* entry = find_entry(state, key);
    if(entry->nil_p()) {
      return cUndef;
    }
    return entry->value();
  }

  Object* LookupTable::remove(STATE, Object* key, bool* removed) {
    hashval bin;
    LookupTableBucket* entry;
    LookupTableBucket* last = NULL;

    size_t num_entries = entries_->to_native();
    size_t num_bins = bins_->to_native();

    if(min_density_p(num_entries, num_bins) && (num_bins >> 1) >= LOOKUPTABLE_MIN_SIZE) {
      redistribute(state, num_bins >>= 1);
    }

    key_to_sym(key);
    bin = find_bin(key_hash(key), num_bins);
    entry = try_as<LookupTableBucket>(values_->at(state, bin));

    while(entry) {
      if(entry->key() == key) {
        Object *val = entry->value();
        if(last) {
          last->next(state, entry->next());
        } else {
          values_->put(state, bin, entry->next());
        }
        entries(state, Fixnum::from(entries_->to_native() - 1));
        if(removed) *removed = true;
        return val;
      }

      last = entry;
      entry = try_as<LookupTableBucket>(entry->next());
    }

    return cNil;
  }

  Object* LookupTable::has_key(STATE, Object* key) {
    LookupTableBucket* entry = find_entry(state, key);

    if(entry->nil_p()) return cFalse;
    return cTrue;
  }

  Array* LookupTable::collect(STATE, LookupTable* tbl, CollectAction& action, Array* ary)
  {
    size_t i, j;
    Tuple* values;

    if(!ary) ary = Array::create(state, tbl->entries()->to_native());

    size_t num_bins = tbl->bins()->to_native();
    values = tbl->values();

    for(i = j = 0; i < num_bins; i++) {
      LookupTableBucket* entry = try_as<LookupTableBucket>(values->at(state, i));

      while(entry) {
        Object* ret = action.call(state, entry);
        if(ret) ary->append(state, ret);
        entry = try_as<LookupTableBucket>(entry->next());
      }
    }
    return ary;
  }

  Array* LookupTable::collect(STATE, LookupTable* tbl,
                              Object* (*action)(STATE, LookupTableBucket*))
  {
    size_t i, j;
    Tuple* values;

    Array* ary = Array::create(state, tbl->entries()->to_native());
    size_t num_bins = tbl->bins()->to_native();
    values = tbl->values();

    for(i = j = 0; i < num_bins; i++) {
      LookupTableBucket* entry = try_as<LookupTableBucket>(values->at(state, i));

      while(entry) {
        ary->set(state, j++, action(state, entry));
        entry = try_as<LookupTableBucket>(entry->next());
      }
    }
    return ary;
  }

  Object* LookupTable::get_key(STATE, LookupTableBucket* entry) {
    return entry->key();
  }

  Array* LookupTable::all_keys(STATE) {
    class all_keys : public CollectAction {
    public:
      virtual Object* call(STATE, LookupTableBucket* bucket) {
        return bucket->key();
      }
    } match;

    return collect(state, this, match);
  }

  Array* LookupTable::filtered_keys(STATE, ObjectMatcher& matcher, Array* ary) {
    class filtered_keys : public CollectAction {
      ObjectMatcher& m_;

    public:
      filtered_keys(ObjectMatcher& m)
        : m_(m)
      {}

      virtual Object* call(STATE, LookupTableBucket* bucket) {
        if(m_.match_p(state, bucket->key())) return bucket->key();
        return 0;
      }
    } match(matcher);

    return collect(state, this, match, ary);
  }

  Object* LookupTable::get_value(STATE, LookupTableBucket* entry) {
    return entry->value();
  }

  Array* LookupTable::all_values(STATE) {
    return collect(state, this, get_value);
  }

  Object* LookupTable::get_entry(STATE, LookupTableBucket* entry) {
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
      if(Symbol* sym = try_as<Symbol>(keys->get(state, i))) {
        std::cout << ":" << sym->debug_str(state);
      } else if(Fixnum* fix = try_as<Fixnum>(keys->get(state, i))) {
        std::cout << fix->to_native();
      }
      if(i < size - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    close_body(level);
  }

  LookupTableBucket* LookupTableBucket::create(STATE, Object *key, Object *value) {
    LookupTableBucket *entry =
      state->new_object<LookupTableBucket>(G(lookuptablebucket));

    entry->key(state, key);
    entry->value(state, value);
    return entry;
  }

  Object* LookupTableBucket::append(STATE, LookupTableBucket* nxt) {
    LookupTableBucket* cur = try_as<LookupTableBucket>(this->next());
    LookupTableBucket* last = this;

    while(cur) {
      last = cur;
      cur = try_as<LookupTableBucket>(cur->next());
    }

    last->next(state, nxt);
    return nxt;
  }

}
