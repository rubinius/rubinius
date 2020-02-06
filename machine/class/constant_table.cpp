#include "object_utils.hpp"
#include "memory.hpp"

#include "class/array.hpp"
#include "class/autoload.hpp"
#include "class/class.hpp"
#include "class/constant_table.hpp"
#include "class/fixnum.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"
#include "class/thread.hpp"

#include <mutex>

#define CONSTANT_TABLE_MAX_DENSITY 0.75
#define CONSTANT_TABLE_MIN_DENSITY 0.3

#define key_hash(obj) (((unsigned int)(uintptr_t)obj))
#define find_bin(hash, bins) (hash & ((bins) - 1))
#define max_density_p(ents,bins) (ents >= CONSTANT_TABLE_MAX_DENSITY * bins)
#define min_density_p(ents,bins) (ents < CONSTANT_TABLE_MIN_DENSITY * bins)


namespace rubinius {
  void ConstantTable::bootstrap(STATE) {
    GO(constant_table).set(
        Class::bootstrap_class(state, G(object), ConstantTableType));
    GO(constant_table_bucket).set(
        Class::bootstrap_class(state, G(object), ConstantTableBucketType));
  }

  ConstantTable* ConstantTable::create(STATE, intptr_t size) {
    ConstantTable *tbl;

    tbl = state->memory()->new_object<ConstantTable>(state, G(constant_table));
    tbl->setup(state, size);

    return tbl;
  }

  void ConstantTable::setup(STATE, intptr_t size) {
    if(size > 0) {
      values(state, Tuple::create(state, size));
      bins(state, Fixnum::from(size));
    }
  }

  /* The ConstantTable.allocate primitive. */
  ConstantTable* ConstantTable::allocate(STATE, Object* self) {
    ConstantTable* tbl = create(state);
    tbl->klass(state, as<Class>(self));
    new(&tbl->lock_) locks::spinlock_mutex;
    return tbl;
  }

  ConstantTable* ConstantTable::duplicate(STATE) {
    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    intptr_t size = bins()->to_native();
    ConstantTable* dup = ConstantTable::create(state, size);

    // Allow for subclassing.
    dup->klass(state, class_object(state));

    intptr_t num = bins()->to_native();

    for(intptr_t i = 0; i < num; i++) {
      ConstantTableBucket* entry = try_as<ConstantTableBucket>(
          table_values(state)->at(state, i));

      while(entry) {
        dup->store(state, entry->name(), entry->constant(), entry->visibility());
        entry = try_as<ConstantTableBucket>(entry->next());
      }
    }

    return dup;
  }

  void ConstantTable::redistribute(STATE, intptr_t size) {
    intptr_t num = bins()->to_native();
    Tuple* new_values = Tuple::create(state, size);

    for(intptr_t i = 0; i < num; i++) {
      ConstantTableBucket* entry = try_as<ConstantTableBucket>(
          table_values(state)->at(state, i));

      while(entry) {
        ConstantTableBucket* link = try_as<ConstantTableBucket>(entry->next());
        entry->next(state, nil<ConstantTableBucket>());

        intptr_t bin = find_bin(key_hash(entry->name()), size);
        ConstantTableBucket* slot = try_as<ConstantTableBucket>(new_values->at(state, bin));

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

  Tuple* ConstantTable::table_values(STATE) {
    if(values()->nil_p()) {
      values(state, Tuple::create(state, CONSTANT_TABLE_MIN_SIZE));
      bins(state, Fixnum::from(CONSTANT_TABLE_MIN_SIZE));
    }

    return values();
  }

  Object* ConstantTable::store(STATE, Symbol* name, Object* constant, Symbol* vis) {
    check_frozen(state);

    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    Tuple* values = table_values(state);

    intptr_t num_entries = entries()->to_native();
    intptr_t num_bins = bins()->to_native();

    if(max_density_p(num_entries, num_bins)) {
      redistribute(state, num_bins <<= 1);
      values = table_values(state);
    }

    intptr_t bin = find_bin(key_hash(name), num_bins);

    ConstantTableBucket* entry = try_as<ConstantTableBucket>(values->at(state, bin));
    ConstantTableBucket* last = NULL;

    while(entry) {
      if(entry->name() == name) {
        entry->constant(state, constant);
        return name;
      }

      last = entry;
      entry = try_as<ConstantTableBucket>(entry->next());
    }

    if(last) {
      last->next(state, ConstantTableBucket::create(state, name, constant, vis));
    } else {
      values->put(state, bin, ConstantTableBucket::create(state, name, constant, vis));
    }

    entries(state, Fixnum::from(num_entries + 1));

    return name;
  }

  ConstantTableBucket* ConstantTable::find_entry(STATE, Symbol* name) {
    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    if(bins()->to_native() == 0) return 0;

    intptr_t bin = find_bin(key_hash(name), bins()->to_native());
    ConstantTableBucket *entry = try_as<ConstantTableBucket>(
        table_values(state)->at(state, bin));

    while(entry) {
      if(entry->name() == name) {
        return entry;
      }
      entry = try_as<ConstantTableBucket>(entry->next());
    }

    return 0;
  }

  ConstantTableBucket* ConstantTable::lookup(STATE, Symbol* name) {
    if(ConstantTableBucket* bucket = find_entry(state, name)) {
      return bucket;
    }

    return nil<ConstantTableBucket>();
  }

  Object* ConstantTable::remove(STATE, Symbol* name) {
    check_frozen(state);

    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    intptr_t num_entries = entries()->to_native();
    intptr_t num_bins = bins()->to_native();

    if(min_density_p(num_entries, num_bins) &&
         (num_bins >> 1) >= CONSTANT_TABLE_MIN_SIZE) {
      redistribute(state, num_bins >>= 1);
    }

    intptr_t bin = find_bin(key_hash(name), num_bins);
    ConstantTableBucket* entry = try_as<ConstantTableBucket>(
        table_values(state)->at(state, bin));
    ConstantTableBucket* last = NULL;

    while(entry) {
      if(entry->name() == name) {
        Object* val = entry->constant();
        if(last) {
          last->next(state, entry->next());
        } else {
          table_values(state)->put(state, bin, entry->next());
        }

        entries(state, Fixnum::from(entries()->to_native() - 1));
        return val;
      }

      last = entry;
      entry = try_as<ConstantTableBucket>(entry->next());
    }

    return cNil;
  }

  Object* ConstantTable::has_name(STATE, Symbol* name) {
    ConstantTableBucket* entry = find_entry(state, name);

    if(!entry) return cFalse;
    return cTrue;
  }

  Array* ConstantTable::all_keys(STATE) {
    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    Array* ary = Array::create(state, entries()->to_native());

    intptr_t num_bins = bins()->to_native();

    for(intptr_t i = 0; i < num_bins; i++) {
      ConstantTableBucket* entry = try_as<ConstantTableBucket>(
          table_values(state)->at(state, i));

      while(entry) {
        ary->append(state, entry->name());
        entry = try_as<ConstantTableBucket>(entry->next());
      }
    }
    return ary;
  }

  void ConstantTable::Info::show(STATE, Object* self, int level) {
    ConstantTable* tbl = as<ConstantTable>(self);
    intptr_t size = tbl->bins()->to_native();

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    indent(++level);
    for(intptr_t i = 0; i < size; i++) {
      ConstantTableBucket* entry = try_as<ConstantTableBucket>(tbl->values()->at(state, i));

      bool entries = false;
      while(entry) {
        if(entries) std::cout << ", ";

        if(Symbol* sym = try_as<Symbol>(entry->name())) {
          std::cout << ":" << sym->debug_str(state);
        } else if(Fixnum* fix = try_as<Fixnum>(entry->name())) {
          std::cout << fix->to_native() << ", ";
        }
        entry = try_as<ConstantTableBucket>(entry->next());

        entries = true;
      }
    }
    std::cout << std::endl;
    close_body(level);
  }

  ConstantTableBucket* ConstantTableBucket::create(STATE, Symbol* name,
      Object* constant, Symbol* vis)
  {
    ConstantTableBucket *entry =
      state->memory()->new_object<ConstantTableBucket>(state, G(constant_table_bucket));

    entry->name(state, name);
    entry->constant(state, constant);
    entry->visibility(state, vis);

    return entry;
  }

  Object* ConstantTableBucket::append(STATE, ConstantTableBucket* nxt) {
    ConstantTableBucket* cur = try_as<ConstantTableBucket>(this->next());
    ConstantTableBucket* last = this;

    while(cur) {
      last = cur;
      cur = try_as<ConstantTableBucket>(cur->next());
    }

    last->next(state, nxt);
    return nxt;
  }

  bool ConstantTableBucket::private_p(STATE) {
    return visibility() == G(sym_private);
  }

  bool ConstantTableBucket::protected_p(STATE) {
    return visibility() == G(sym_protected);
  }

  bool ConstantTableBucket::public_p(STATE) {
    return visibility() == G(sym_public);
  }

  bool ConstantTableBucket::undef_p(STATE) {
    return visibility() == G(sym_undef);
  }
}

