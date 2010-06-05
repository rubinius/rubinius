#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/executable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/alias.hpp"

#include <iostream>

#define METHODTABLE_MAX_DENSITY 0.75
#define METHODTABLE_MIN_DENSITY 0.3

#define key_hash(obj) (((unsigned int)(uintptr_t)obj))
#define find_bin(hash, bins) (hash & ((bins) - 1))
#define max_density_p(ents,bins) (ents >= METHODTABLE_MAX_DENSITY * bins)
#define min_density_p(ents,bins) (ents < METHODTABLE_MIN_DENSITY * bins)


namespace rubinius {
  MethodTable* MethodTable::create(STATE, size_t size) {
    MethodTable *tbl;

    tbl = state->new_object<MethodTable>(G(methtbl));
    tbl->setup(state, size);

    return tbl;
  }

  void MethodTable::setup(STATE, size_t sz = 0) {
    if(!sz) sz = METHODTABLE_MIN_SIZE;
    values(state, Tuple::create(state, sz));
    bins(state, Fixnum::from(sz));
    entries(state, Fixnum::from(0));
  }

  /* The MethodTable.allocate primitive. */
  MethodTable* MethodTable::allocate(STATE, Object* self) {
    MethodTable* tbl = create(state, METHODTABLE_MIN_SIZE);
    tbl->klass(state, as<Class>(self));
    return tbl;
  }

  MethodTable* MethodTable::duplicate(STATE) {
    size_t size, i;
    MethodTable *dup;

    size = bins_->to_native();
    dup = MethodTable::create(state, size);

    // Allow for subclassing.
    dup->klass(state, class_object(state));

    size_t num = entries_->to_native();

    Array* entries = all_entries(state);
    for(i = 0; i < num; i++) {
      MethodTableBucket* entry = as<MethodTableBucket>(entries->get(state, i));
      dup->store(state, entry->name(), entry->method(), entry->visibility());
    }
    return dup;
  }

  void MethodTable::redistribute(STATE, size_t size) {
    size_t num = bins_->to_native();
    Tuple* new_values = Tuple::create(state, size);

    for(size_t i = 0; i < num; i++) {
      MethodTableBucket* entry = try_as<MethodTableBucket>(values_->at(state, i));

      while(entry) {
        MethodTableBucket* link = try_as<MethodTableBucket>(entry->next());
        entry->next(state, reinterpret_cast<MethodTableBucket *>(Qnil));

        size_t bin = find_bin(key_hash(entry->name()), size);
        MethodTableBucket* slot = try_as<MethodTableBucket>(new_values->at(state, bin));

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

  Object* MethodTable::store(STATE, Symbol* name, Object* exec, Symbol* vis) {
    unsigned int num_entries, num_bins, bin;
    MethodTableBucket* entry;
    MethodTableBucket* last = NULL;

    Executable* method;
    if(exec->nil_p()) {
      method = reinterpret_cast<Executable*>(Qnil);
    } else {
      if(Alias* alias = try_as<Alias>(exec)) {
        method = alias->original_exec();
      } else {
        method = as<Executable>(exec);
      }
    }

    num_entries = entries_->to_native();
    num_bins = bins_->to_native();

    if(max_density_p(num_entries, num_bins)) {
      redistribute(state, num_bins <<= 1);
    }

    bin = find_bin(key_hash(name), num_bins);
    entry = try_as<MethodTableBucket>(values_->at(state, bin));

    while(entry) {
      if(entry->name() == name) {
        entry->method(state, method);
        entry->visibility(state, vis);
        return name;
      }
      last = entry;
      entry = try_as<MethodTableBucket>(entry->next());
    }

    if(last) {
      last->next(state, MethodTableBucket::create(state, name, method, vis));
    } else {
      values_->put(state, bin, MethodTableBucket::create(state, name, method, vis));
    }

    entries(state, Fixnum::from(num_entries + 1));
    return name;
  }

  Object* MethodTable::alias(STATE, Symbol* name, Symbol* vis,
                             Symbol* orig_name, Executable* orig_method,
                             Module* orig_mod)
  {
    unsigned int num_entries, num_bins, bin;
    MethodTableBucket* entry;
    MethodTableBucket* last = NULL;

    if(Alias* alias = try_as<Alias>(orig_method)) {
      orig_method = alias->original_exec();
      orig_mod = alias->original_module();
      orig_name = alias->original_name();
    }

    Alias* method = Alias::create(state, orig_name, orig_mod, orig_method);

    num_entries = entries_->to_native();
    num_bins = bins_->to_native();

    if(max_density_p(num_entries, num_bins)) {
      redistribute(state, num_bins <<= 1);
    }

    bin = find_bin(key_hash(name), num_bins);
    entry = try_as<MethodTableBucket>(values_->at(state, bin));

    while(entry) {
      if(entry->name() == name) {
        entry->method(state, method);
        entry->visibility(state, vis);
        return name;
      }
      last = entry;
      entry = try_as<MethodTableBucket>(entry->next());
    }

    if(last) {
      last->next(state, MethodTableBucket::create(state, name, method, vis));
    } else {
      values_->put(state, bin, MethodTableBucket::create(state, name, method, vis));
    }

    entries(state, Fixnum::from(num_entries + 1));
    return name;
  }
  MethodTableBucket* MethodTable::find_entry(STATE, Symbol* name) {
    unsigned int bin;

    bin = find_bin(key_hash(name), bins_->to_native());
    MethodTableBucket *entry = try_as<MethodTableBucket>(values_->at(state, bin));

    while(entry) {
      if(entry->name() == name) {
        return entry;
      }
      entry = try_as<MethodTableBucket>(entry->next());
    }

    return 0;
  }

  MethodTableBucket* MethodTable::find_entry(Symbol* name) {
    unsigned int bin;

    bin = find_bin(key_hash(name), bins_->to_native());
    MethodTableBucket *entry = try_as<MethodTableBucket>(values_->at(bin));

    while(entry) {
      if(entry->name() == name) {
        return entry;
      }
      entry = try_as<MethodTableBucket>(entry->next());
    }

    return 0;
  }

  MethodTableBucket* MethodTable::lookup(STATE, Symbol* name) {
    if(MethodTableBucket* bucket = find_entry(state, name)) {
      return bucket;
    }

    return reinterpret_cast<MethodTableBucket*>(Qnil);
  }

  Executable* MethodTable::remove(STATE, Symbol* name) {
    hashval bin;
    MethodTableBucket* entry;
    MethodTableBucket* last = NULL;

    size_t num_entries = entries_->to_native();
    size_t num_bins = bins_->to_native();

    if(min_density_p(num_entries, num_bins) && (num_bins >> 1) >= METHODTABLE_MIN_SIZE) {
      redistribute(state, num_bins >>= 1);
    }

    bin = find_bin(key_hash(name), num_bins);
    entry = try_as<MethodTableBucket>(values_->at(state, bin));

    while(entry) {
      if(entry->name() == name) {
        Executable* val = entry->method();
        if(last) {
          last->next(state, entry->next());
        } else {
          values_->put(state, bin, entry->next());
        }
        entries(state, Fixnum::from(entries_->to_native() - 1));
        return val;
      }

      last = entry;
      entry = try_as<MethodTableBucket>(entry->next());
    }

    return reinterpret_cast<Executable*>(Qnil);
  }

  Object* MethodTable::has_name(STATE, Symbol* name) {
    MethodTableBucket* entry = find_entry(state, name);

    if(!entry) return Qfalse;
    return Qtrue;
  }

  Array* MethodTable::collect(STATE, MethodTable* tbl,
                              Object* (*action)(STATE, MethodTableBucket*))
  {
    size_t i, j;
    Tuple* values;
    MethodTableBucket* entry;

    Array* ary = Array::create(state, tbl->entries()->to_native());
    size_t num_bins = tbl->bins()->to_native();
    values = tbl->values();

    for(i = j = 0; i < num_bins; i++) {
      entry = try_as<MethodTableBucket>(values->at(state, i));

      while(entry) {
        ary->set(state, j++, action(state, entry));
        entry = try_as<MethodTableBucket>(entry->next());
      }
    }
    return ary;
  }

  Object* MethodTable::get_name(STATE, MethodTableBucket* entry) {
    return entry->name();
  }

  Array* MethodTable::all_names(STATE) {
    return collect(state, this, get_name);
  }

  Object* MethodTable::get_value(STATE, MethodTableBucket* entry) {
    return entry->method();
  }

  Array* MethodTable::all_values(STATE) {
    return collect(state, this, get_value);
  }

  Object* MethodTable::get_entry(STATE, MethodTableBucket* entry) {
    return entry;
  }

  Array* MethodTable::all_entries(STATE) {
    return collect(state, this, get_entry);
  }

  void MethodTable::Info::show(STATE, Object* self, int level) {
    MethodTable* tbl = as<MethodTable>(self);
    size_t size = tbl->entries()->to_native();
    Array* names = tbl->all_names(state);

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    indent(++level);
    for(size_t i = 0; i < size; i++) {
      if(Symbol* sym = try_as<Symbol>(names->get(state, i))) {
        std::cout << ":" << sym->c_str(state);
      } else if(Fixnum* fix = try_as<Fixnum>(names->get(state, i))) {
        std::cout << fix->to_native();
      }
      if(i < size - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    close_body(level);
  }

  MethodTableBucket* MethodTableBucket::create(STATE, Symbol* name,
      Executable* method, Symbol* vis)
  {
    MethodTableBucket *entry =
      state->new_object<MethodTableBucket>(G(methtblbucket));

    entry->name(state, name);
    entry->method(state, method);
    entry->visibility(state, vis);
    return entry;
  }

  Object* MethodTableBucket::append(STATE, MethodTableBucket* nxt) {
    MethodTableBucket* cur = try_as<MethodTableBucket>(this->next());
    MethodTableBucket* last = this;

    while(cur) {
      last = cur;
      cur = try_as<MethodTableBucket>(cur->next());
    }

    last->next(state, nxt);
    return nxt;
  }

  bool MethodTableBucket::private_p(STATE) {
    return visibility_ == G(sym_private);
  }

  bool MethodTableBucket::protected_p(STATE) {
    return visibility_ == G(sym_protected);
  }

  bool MethodTableBucket::public_p(STATE) {
    return visibility_ == G(sym_public);
  }

  bool MethodTableBucket::undef_p(STATE) {
    return visibility_ == G(sym_undef);
  }
}
