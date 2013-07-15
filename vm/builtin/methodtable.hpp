#ifndef RBX_BUILTIN_METHODTABLE_HPP
#define RBX_BUILTIN_METHODTABLE_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class Tuple;
  class Array;
  class Executable;
  class Symbol;

  class MethodTableBucket : public Object {
  public:
    const static object_type type = MethodTableBucketType;

  private:
    Symbol* name_;   // slot
    Symbol* visibility_; // slot
    Executable* method_; // slot
    MethodTableBucket* next_;  // slot

  public:
    attr_accessor(name, Symbol);
    attr_accessor(visibility, Symbol);
    attr_accessor(method, Executable);
    attr_accessor(next, MethodTableBucket);

    static MethodTableBucket* create(STATE, Symbol* name,
        Executable* method, Symbol* visibility);

    Object* append(STATE, MethodTableBucket *nxt);

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

  private:
    Tuple* values_;   // slot
    Integer* bins_;    // slot
    Integer* entries_; // slot
    utilities::thread::SpinLock lock_;

    void   redistribute(STATE, size_t size);

  public:
    /* accessors */

    attr_accessor(values, Tuple);
    attr_accessor(bins, Integer);
    attr_accessor(entries, Integer);

    /* interface */

    static MethodTable* create(STATE, size_t sz = METHODTABLE_MIN_SIZE);
    void setup(STATE, size_t sz);

    // Rubinius.primitive :methodtable_allocate
    static MethodTable* allocate(STATE, Object* self);

    // Rubinius.primitive :methodtable_store
    Object* store(STATE, Symbol* name, Object* meth, Symbol* vis);

    // Rubinius.primitive :methodtable_alias
    Object* alias(STATE, Symbol* name, Symbol* vis, Symbol* orig_name, Object* orig_method, Module* orig_mod);

    // Rubinius.primitive :methodtable_duplicate
    MethodTable* duplicate(STATE);

    MethodTableBucket* find_entry(STATE, Symbol* name);
    MethodTableBucket* find_entry(Symbol* name);

    // Rubinius.primitive+ :methodtable_lookup
    MethodTableBucket* lookup(STATE, Symbol* name);

    // Rubinius.primitive :methodtable_delete
    Executable* remove(STATE, Symbol* name);

    // Rubinius.primitive+ :methodtable_has_name
    Object* has_name(STATE, Symbol* name);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
    };
  };
}

#endif
