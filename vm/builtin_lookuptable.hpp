#ifndef RBX_BUILTIN_LOOKUPTABLE_HPP
#define RBX_BUILTIN_LOOKUPTABLE_HPP

namespace rubinius {
  #define LOOKUPTABLE_MIN_SIZE 16
  class LookupTable : public BuiltinType {
    public:
    const static size_t fields = 4;
    const static object_type type = LookupTableType;

    OBJECT __ivars__; // slot
    Tuple* values; // slot
    INTEGER bins; // slot
    INTEGER entries; // slot

    /* Prototypes */
    static LookupTable* create(STATE, size_t sz = LOOKUPTABLE_MIN_SIZE);
    void setup(STATE, size_t sz);
    OBJECT store(STATE, OBJECT key, OBJECT val);
    OBJECT fetch(STATE, OBJECT key);
    OBJECT fetch(STATE, OBJECT key, bool* found);
    LookupTable* dup(STATE);
    static OBJECT entry_new(STATE, OBJECT key, OBJECT val);
    static OBJECT entry_append(STATE, Tuple* top, OBJECT nxt);
    void   redistribute(STATE, size_t size);
    Tuple* find_entry(STATE, OBJECT key);
    OBJECT find(STATE, OBJECT key);
    OBJECT remove(STATE, OBJECT key);
    OBJECT has_key(STATE, OBJECT key);
    static Array* collect(STATE, LookupTable* tbl, OBJECT (*action)(STATE, Tuple*));
    static OBJECT get_key(STATE, Tuple* entry);
    Array* all_keys(STATE);
    static OBJECT get_value(STATE, Tuple* entry);
    Array* all_values(STATE);
    static OBJECT get_entry(STATE, Tuple* entry);
    Array* all_entries(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void set_field(STATE, OBJECT target, size_t index, OBJECT val);
      virtual OBJECT get_field(STATE, OBJECT target, size_t index);
    };
  };

};

#endif
