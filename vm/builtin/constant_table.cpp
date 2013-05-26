#include "builtin/array.hpp"
#include "builtin/autoload.hpp"
#include "builtin/class.hpp"
#include "builtin/constant_table.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/thread.hpp"
#include "object_utils.hpp"

#define CONSTANT_TABLE_MAX_DENSITY 0.75
#define CONSTANT_TABLE_MIN_DENSITY 0.3

#define key_hash(obj) (((unsigned int)(uintptr_t)obj))
#define find_bin(hash, bins) (hash & ((bins) - 1))
#define max_density_p(ents,bins) (ents >= CONSTANT_TABLE_MAX_DENSITY * bins)
#define min_density_p(ents,bins) (ents < CONSTANT_TABLE_MIN_DENSITY * bins)


namespace rubinius {
  ConstantTable* ConstantTable::create(STATE, native_int size) {
    ConstantTable *tbl;

    tbl = state->new_object<ConstantTable>(G(constant_table));
    tbl->setup(state, size);
    tbl->lock_.init();

    return tbl;
  }

  void ConstantTable::setup(STATE, native_int sz = 0) {
    if(!sz) sz = CONSTANT_TABLE_MIN_SIZE;
    values(state, Tuple::create(state, sz));
    bins(state, Fixnum::from(sz));
    entries(state, Fixnum::from(0));
  }

  /* The ConstantTable.allocate primitive. */
  ConstantTable* ConstantTable::allocate(STATE, Object* self) {
    ConstantTable* tbl = create(state, CONSTANT_TABLE_MIN_SIZE);
    tbl->klass(state, as<Class>(self));
    tbl->lock_.init();
    return tbl;
  }

  ConstantTable* ConstantTable::duplicate(STATE) {
    utilities::thread::SpinLock::LockGuard lg(lock_);

    native_int size = bins_->to_native();
    ConstantTable* dup = ConstantTable::create(state, size);

    // Allow for subclassing.
    dup->klass(state, class_object(state));

    native_int num = bins_->to_native();

    for(native_int i = 0; i < num; i++) {
      ConstantTableBucket* entry = try_as<ConstantTableBucket>(values_->at(state, i));

      while(entry) {
        dup->store(state, entry->name(), entry->constant(), entry->visibility());
        entry = try_as<ConstantTableBucket>(entry->next());
      }
    }

    return dup;
  }

  void ConstantTable::redistribute(STATE, native_int size) {
    native_int num = bins_->to_native();
    Tuple* new_values = Tuple::create(state, size);

    for(native_int i = 0; i < num; i++) {
      ConstantTableBucket* entry = try_as<ConstantTableBucket>(values_->at(state, i));

      while(entry) {
        ConstantTableBucket* link = try_as<ConstantTableBucket>(entry->next());
        entry->next(state, nil<ConstantTableBucket>());

        native_int bin = find_bin(key_hash(entry->name()), size);
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

  Object* ConstantTable::store(STATE, Symbol* name, Object* constant, Symbol* vis) {
    check_frozen(state);

    utilities::thread::SpinLock::LockGuard lg(lock_);

    native_int num_entries = entries_->to_native();
    native_int num_bins = bins_->to_native();

    if(max_density_p(num_entries, num_bins)) {
      redistribute(state, num_bins <<= 1);
    }

    native_int bin = find_bin(key_hash(name), num_bins);

    ConstantTableBucket* entry = try_as<ConstantTableBucket>(values_->at(state, bin));
    ConstantTableBucket* last = NULL;

    while(entry) {
      if(entry->name() == name) {
        if(Autoload* autoload = try_as<Autoload>(entry->constant())) {
          autoload->constant(state, constant);
          autoload->thread(state, Thread::current(state));
        } else {
          entry->constant(state, constant);
        }
        return name;
      }

      last = entry;
      entry = try_as<ConstantTableBucket>(entry->next());
    }

    if(last) {
      last->next(state, ConstantTableBucket::create(state, name, constant, vis));
    } else {
      values_->put(state, bin,
                         ConstantTableBucket::create(state, name, constant, vis));
    }

    entries(state, Fixnum::from(num_entries + 1));

    return name;
  }

  ConstantTableBucket* ConstantTable::find_entry(STATE, Symbol* name) {
    utilities::thread::SpinLock::LockGuard lg(lock_);

    native_int bin = find_bin(key_hash(name), bins_->to_native());
    ConstantTableBucket *entry = try_as<ConstantTableBucket>(values_->at(state, bin));

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

    utilities::thread::SpinLock::LockGuard lg(lock_);

    native_int num_entries = entries_->to_native();
    native_int num_bins = bins_->to_native();

    if(min_density_p(num_entries, num_bins) &&
         (num_bins >> 1) >= CONSTANT_TABLE_MIN_SIZE) {
      redistribute(state, num_bins >>= 1);
    }

    native_int bin = find_bin(key_hash(name), num_bins);
    ConstantTableBucket* entry = try_as<ConstantTableBucket>(values_->at(state, bin));
    ConstantTableBucket* last = NULL;

    while(entry) {
      if(entry->name() == name) {
        Object* val = entry->constant();
        if(last) {
          last->next(state, entry->next());
        } else {
          values_->put(state, bin, entry->next());
        }

        entries(state, Fixnum::from(entries_->to_native() - 1));
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
    utilities::thread::SpinLock::LockGuard lg(lock_);

    Array* ary = Array::create(state, entries_->to_native());

    native_int num_bins = bins_->to_native();

    for(native_int i = 0; i < num_bins; i++) {
      ConstantTableBucket* entry = try_as<ConstantTableBucket>(values_->at(state, i));

      while(entry) {
        ary->append(state, entry->name());
        entry = try_as<ConstantTableBucket>(entry->next());
      }
    }
    return ary;
  }

  void ConstantTable::Info::show(STATE, Object* self, int level) {
    ConstantTable* tbl = as<ConstantTable>(self);
    native_int size = tbl->bins()->to_native();

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    indent(++level);
    for(native_int i = 0; i < size; i++) {
      ConstantTableBucket* entry = try_as<ConstantTableBucket>(tbl->values()->at(state, i));

      while(entry) {
       if(Symbol* sym = try_as<Symbol>(entry->name())) {
          std::cout << ":" << sym->debug_str(state);
        } else if(Fixnum* fix = try_as<Fixnum>(entry->name())) {
          std::cout << fix->to_native();
        }
        entry = try_as<ConstantTableBucket>(entry->next());
      }
      if(i < size - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    close_body(level);
  }

  ConstantTableBucket* ConstantTableBucket::create(STATE, Symbol* name,
      Object* constant, Symbol* vis)
  {
    ConstantTableBucket *entry =
      state->new_object<ConstantTableBucket>(G(constant_table_bucket));

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
    return visibility_ == G(sym_private);
  }

  bool ConstantTableBucket::protected_p(STATE) {
    return visibility_ == G(sym_protected);
  }

  bool ConstantTableBucket::public_p(STATE) {
    return visibility_ == G(sym_public);
  }

  bool ConstantTableBucket::undef_p(STATE) {
    return visibility_ == G(sym_undef);
  }
}

