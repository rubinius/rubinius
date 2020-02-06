#include "memory.hpp"
#include "object_utils.hpp"

#include "class/alias.hpp"
#include "class/class.hpp"
#include "class/code_db.hpp"
#include "class/compiled_code.hpp"
#include "class/lexical_scope.hpp"
#include "class/executable.hpp"
#include "class/fixnum.hpp"
#include "class/method_table.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"

#include <mutex>

#define METHODTABLE_MAX_DENSITY 0.75
#define METHODTABLE_MIN_DENSITY 0.3

#define key_hash(obj) (((unsigned int)(uintptr_t)obj))
#define find_bin(hash, bins) (hash & ((bins) - 1))
#define max_density_p(ents,bins) (ents >= METHODTABLE_MAX_DENSITY * bins)
#define min_density_p(ents,bins) (ents < METHODTABLE_MIN_DENSITY * bins)


namespace rubinius {
  void MethodTable::bootstrap(STATE) {
    GO(method_table).set(
        Class::bootstrap_class(state, G(object), MethodTableType));
    GO(method_table_bucket).set(
        Class::bootstrap_class(state, G(object), MethodTableBucketType));
  }

  MethodTable* MethodTable::create(STATE, size_t size) {
    MethodTable *tbl;

    tbl = state->memory()->new_object<MethodTable>(state, G(method_table));
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
    new(&tbl->lock_) locks::spinlock_mutex;
    return tbl;
  }

  MethodTable* MethodTable::duplicate(STATE) {
    size_t size, i;
    MethodTable* dup = 0;

    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    size = bins()->to_native();
    dup = MethodTable::create(state, size);

    // Allow for subclassing.
    dup->klass(state, class_object(state));

    size_t num = bins()->to_native();
    for(i = 0; i < num; i++) {
      MethodTableBucket* entry = try_as<MethodTableBucket>(values()->at(state, i));

      while(entry) {
        dup->store(state, entry->name(), entry->method_id(),
            entry->method(), entry->scope(), entry->serial(), entry->visibility());
        entry = try_as<MethodTableBucket>(entry->next());
      }
    }

    return dup;
  }

  void MethodTable::redistribute(STATE, size_t size) {
    size_t num = bins()->to_native();
    Tuple* new_values = Tuple::create(state, size);

    for(size_t i = 0; i < num; i++) {
      MethodTableBucket* entry = try_as<MethodTableBucket>(values()->at(state, i));

      while(entry) {
        MethodTableBucket* link = try_as<MethodTableBucket>(entry->next());
        entry->next(state, nil<MethodTableBucket>());

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

  Object* MethodTable::store(STATE, Symbol* name, Object* method_id,
      Object* method, Object* scope, Fixnum* serial, Symbol* visibility)
  {
    check_frozen(state);

    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    if(!method->nil_p()) {
      if(Alias* stored_alias = try_as<Alias>(method)) {
        lock_.unlock();
        Object* res = alias(state, name, visibility,
                           stored_alias->original_name(),
                           stored_alias->original_exec(),
                           stored_alias->original_module());
        lock_.lock();
        return res;
      }
    }

    intptr_t num_entries = entries()->to_native();
    intptr_t num_bins = bins()->to_native();

    if(max_density_p(num_entries, num_bins)) {
      redistribute(state, num_bins <<= 1);
    }

    intptr_t bin = find_bin(key_hash(name), num_bins);

    MethodTableBucket* entry = try_as<MethodTableBucket>(values()->at(state, bin));
    MethodTableBucket* last = NULL;

    while(entry) {
      if(entry->name() == name) {
        entry->method_id(state, method_id);
        entry->method(state, method);
        entry->scope(state, scope);
        entry->serial(state, serial);
        entry->visibility(state, visibility);
        if(!entry->prediction()->nil_p()) {
          entry->prediction()->invalidate();
        }
        entry->prediction(state, nil<Prediction>());
        return name;
      }

      last = entry;
      entry = try_as<MethodTableBucket>(entry->next());
    }

    if(last) {
      last->next(state, MethodTableBucket::create(
            state, name, method_id, method, scope, serial, visibility));
    } else {
      values()->put(state, bin, MethodTableBucket::create(
            state, name, method_id, method, scope, serial, visibility));
    }

    entries(state, Fixnum::from(num_entries + 1));

    return name;
  }

  Object* MethodTable::alias(STATE, Symbol* name, Symbol* vis,
                             Symbol* orig_name, Object* orig_method,
                             Module* orig_mod)
  {
    check_frozen(state);

    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    Executable* orig_exec;

    if(Alias* alias = try_as<Alias>(orig_method)) {
      orig_exec = alias->original_exec();
      orig_mod = alias->original_module();
      orig_name = alias->original_name();
    } else if(orig_method->nil_p()) {
      orig_exec = nil<Executable>();
    } else {
      orig_exec = as<Executable>(orig_method);
    }

    Alias* method = Alias::create(state, orig_name, orig_mod, orig_exec);

    intptr_t num_entries = entries()->to_native();
    intptr_t num_bins = bins()->to_native();

    if(max_density_p(num_entries, num_bins)) {
      redistribute(state, num_bins <<= 1);
    }

    intptr_t bin = find_bin(key_hash(name), num_bins);

    MethodTableBucket* entry = try_as<MethodTableBucket>(values()->at(state, bin));
    MethodTableBucket* last = NULL;

    while(entry) {
      if(entry->name() == name) {
        entry->method_id(state, nil<String>());
        entry->method(state, method);
        entry->scope(state, cNil);
        entry->serial(state, Fixnum::from(0));
        entry->visibility(state, vis);
        if(!entry->prediction()->nil_p()) {
          entry->prediction()->invalidate();
        }
        entry->prediction(state, nil<Prediction>());
        return name;
      }

      last = entry;
      entry = try_as<MethodTableBucket>(entry->next());
    }

    if(last) {
      last->next(state, MethodTableBucket::create(
            state, name, nil<String>(), method, cNil, Fixnum::from(0), vis));
    } else {
      values()->put(state, bin, MethodTableBucket::create(
            state, name, nil<String>(), method, cNil, Fixnum::from(0), vis));
    }

    entries(state, Fixnum::from(num_entries + 1));
    return name;
  }

  MethodTableBucket* MethodTable::find_entry(STATE, Symbol* name) {
    unsigned int bin;
    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    bin = find_bin(key_hash(name), bins()->to_native());
    MethodTableBucket *entry = try_as<MethodTableBucket>(values()->at(state, bin));

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
    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    bin = find_bin(key_hash(name), bins()->to_native());
    MethodTableBucket *entry = try_as<MethodTableBucket>(values()->at(bin));

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

    return nil<MethodTableBucket>();
  }

  Object* MethodTable::remove(STATE, Symbol* name) {
    check_frozen(state);

    std::lock_guard<locks::spinlock_mutex> lg(lock_);

    intptr_t num_entries = entries()->to_native();
    intptr_t num_bins = bins()->to_native();

    if(min_density_p(num_entries, num_bins) &&
         (num_bins >> 1) >= METHODTABLE_MIN_SIZE) {
      redistribute(state, num_bins >>= 1);
    }

    intptr_t bin = find_bin(key_hash(name), num_bins);
    MethodTableBucket* entry = try_as<MethodTableBucket>(values()->at(state, bin));
    MethodTableBucket* last = NULL;

    while(entry) {
      if(entry->name() == name) {
        Object* val = entry->method();
        if(last) {
          last->next(state, entry->next());
        } else {
          values()->put(state, bin, entry->next());
        }

        entries(state, Fixnum::from(entries()->to_native() - 1));
        return val;
      }

      last = entry;
      entry = try_as<MethodTableBucket>(entry->next());
    }

    return nil<Executable>();
  }

  Object* MethodTable::has_name(STATE, Symbol* name) {
    MethodTableBucket* entry = find_entry(state, name);

    if(!entry) return cFalse;
    return cTrue;
  }

  void MethodTable::Info::show(STATE, Object* self, int level) {
    MethodTable* tbl = as<MethodTable>(self);
    size_t size = tbl->bins()->to_native();

    if(size == 0) {
      class_info(state, self, true);
      return;
    }

    class_info(state, self);
    std::cout << ": " << size << std::endl;
    indent(++level);
    for(size_t i = 0; i < size; i++) {
      MethodTableBucket* entry = try_as<MethodTableBucket>(tbl->values()->at(state, i));

      while(entry) {
       if(Symbol* sym = try_as<Symbol>(entry->name())) {
          std::cout << ":" << sym->debug_str(state) << ", ";
        } else if(Fixnum* fix = try_as<Fixnum>(entry->name())) {
          std::cout << fix->to_native() << ", ";
        }
        entry = try_as<MethodTableBucket>(entry->next());
      }
    }
    std::cout << std::endl;
    close_body(level);
  }

  MethodTableBucket* MethodTableBucket::create(STATE, Symbol* name, Object* method_id,
      Object* method, Object* scope, Fixnum* serial, Symbol* vis)
  {
    MethodTableBucket *entry =
      state->memory()->new_object<MethodTableBucket>(state, G(method_table_bucket));

    entry->name(state, name);
    entry->method_id(state, method_id);
    entry->method(state, method);
    entry->scope(state, scope);
    entry->serial(state, serial);
    entry->visibility(state, vis);
    entry->prediction(state, nil<Prediction>());

    return entry;
  }

  Executable* MethodTableBucket::get_method(STATE) {
    if(!method()->nil_p()) return as<Executable>(method());

    if(method_id()->nil_p()) return nil<Executable>();

    if(CompiledCode* code = G(coredb)->load(state, as<String>(method_id()))) {
      if(code->nil_p()) {
        std::string msg = "unable to find code in CodeDB for ID: ";
        msg.append(as<String>(method_id())->c_str(state));

        Exception::raise_runtime_error(state, msg.c_str());
      }

      if(LexicalScope* cs = try_as<LexicalScope>(scope())) {
        code->scope(state, cs);
      } else {
        code->scope(state, nil<LexicalScope>());
      }
      code->serial(state, serial());
      method(state, code);

      return as<Executable>(code);
    }

    return nil<Executable>();
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
    return visibility() == G(sym_private);
  }

  bool MethodTableBucket::protected_p(STATE) {
    return visibility() == G(sym_protected);
  }

  bool MethodTableBucket::public_p(STATE) {
    return visibility() == G(sym_public);
  }

  bool MethodTableBucket::undef_p(STATE) {
    return visibility() == G(sym_undef);
  }
}
