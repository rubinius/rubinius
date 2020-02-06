#ifndef RBX_BUILTIN_METHODTABLE_HPP
#define RBX_BUILTIN_METHODTABLE_HPP

#include "object_utils.hpp"
#include "spinlock.hpp"

#include "class/executable.hpp"
#include "class/object.hpp"
#include "class/prediction.hpp"
#include "class/symbol.hpp"

namespace rubinius {
  class Tuple;
  class Array;
  class String;
  class Symbol;

  class MethodTableBucket : public Object {
  public:
    const static object_type type = MethodTableBucketType;

    attr_accessor(name, Symbol);
    attr_accessor(visibility, Symbol);
    attr_accessor(method_id, Object);
    attr_accessor(method, Object);
    attr_accessor(scope, Object);
    attr_accessor(serial, Fixnum);
    attr_accessor(prediction, Prediction);
    attr_accessor(next, MethodTableBucket);

    static void initialize(STATE, MethodTableBucket* obj) {
      obj->name(nil<Symbol>());
      obj->visibility(nil<Symbol>());
      obj->method_id(nil<Object>());
      obj->method(nil<Executable>());
      obj->scope(nil<Object>());
      obj->serial(nil<Fixnum>());
      obj->prediction(nil<Prediction>());
      obj->next(nil<MethodTableBucket>());
    }

    static MethodTableBucket* create(STATE, Symbol* name, Object* method_id,
        Object* method, Object* scope, Fixnum* serial, Symbol* visibility);

    Object* append(STATE, MethodTableBucket *nxt);

    // Rubinius.primitive :methodtable_bucket_get_method
    Executable* get_method(STATE);

    bool private_p(STATE);
    bool public_p(STATE);
    bool protected_p(STATE);
    bool undef_p(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  #define METHODTABLE_MIN_SIZE 16
  class MethodTable : public Object {
  public:
    const static object_type type = MethodTableType;

    attr_accessor(values, Tuple);
    attr_accessor(bins, Fixnum);
    attr_accessor(entries, Fixnum);

  private:
    locks::spinlock_mutex lock_;

    void   redistribute(STATE, size_t size);

  public:
    static void bootstrap(STATE);
    static void initialize(STATE, MethodTable* obj) {
      obj->values(nil<Tuple>());
      obj->bins(Fixnum::from(0));
      obj->entries(Fixnum::from(0));
      new(&obj->lock_) locks::spinlock_mutex;
    }

    static MethodTable* create(STATE, size_t sz = METHODTABLE_MIN_SIZE);
    void setup(STATE, size_t sz);

    // Rubinius.primitive :methodtable_allocate
    static MethodTable* allocate(STATE, Object* self);

    // Rubinius.primitive :methodtable_store
    Object* store(STATE, Symbol* name, Object* method_id, Object* method, Object* scope, Fixnum* serial, Symbol* vis);

    // Rubinius.primitive :methodtable_alias
    Object* alias(STATE, Symbol* name, Symbol* vis, Symbol* orig_name, Object* orig_method, Module* orig_mod);

    // Rubinius.primitive :methodtable_duplicate
    MethodTable* duplicate(STATE);

    MethodTableBucket* find_entry(STATE, Symbol* name);
    MethodTableBucket* find_entry(Symbol* name);

    // Rubinius.primitive :methodtable_lookup
    MethodTableBucket* lookup(STATE, Symbol* name);

    // Rubinius.primitive :methodtable_delete
    Object* remove(STATE, Symbol* name);

    // Rubinius.primitive :methodtable_has_name
    Object* has_name(STATE, Symbol* name);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
}

#endif
