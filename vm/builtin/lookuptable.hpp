#ifndef RBX_BUILTIN_LOOKUPTABLE_HPP
#define RBX_BUILTIN_LOOKUPTABLE_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Tuple;
  class Array;

  #define LOOKUPTABLE_MIN_SIZE 16
  class LookupTable : public Object {
    public:
    const static size_t fields = 3;
    const static object_type type = LookupTableType;

    Tuple* values; // slot
    INTEGER bins; // slot
    INTEGER entries; // slot

    /* Prototypes */
    static LookupTable* create(STATE, size_t sz = LOOKUPTABLE_MIN_SIZE);
    void setup(STATE, size_t sz);

    // Ruby.primitive :lookuptable_allocate
    static LookupTable* allocate(STATE);

    // Ruby.primitive :lookuptable_store
    OBJECT store(STATE, OBJECT key, OBJECT val);

    // Ruby.primitive :lookuptable_fetch
    OBJECT fetch(STATE, OBJECT key);
    OBJECT fetch(STATE, OBJECT key, bool* found);

    // Ruby.primitive :lookuptable_dup
    LookupTable* dup(STATE);
    static OBJECT entry_new(STATE, OBJECT key, OBJECT val);
    static OBJECT entry_append(STATE, Tuple* top, OBJECT nxt);
    void   redistribute(STATE, size_t size);
    Tuple* find_entry(STATE, OBJECT key);
    OBJECT find(STATE, OBJECT key);
    // Ruby.primitive :lookuptable_delete
    OBJECT remove(STATE, OBJECT key);
    // Ruby.primitive :lookuptable_has_key
    OBJECT has_key(STATE, OBJECT key);
    static Array* collect(STATE, LookupTable* tbl, OBJECT (*action)(STATE, Tuple*));
    static OBJECT get_key(STATE, Tuple* entry);
    // Ruby.primitive :lookuptable_keys
    Array* all_keys(STATE);
    static OBJECT get_value(STATE, Tuple* entry);
    // Ruby.primitive :lookuptable_values
    Array* all_values(STATE);
    static OBJECT get_entry(STATE, Tuple* entry);
    // Ruby.primitive :lookuptable_entries
    Array* all_entries(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, OBJECT self);
    };
  };

};

#endif
