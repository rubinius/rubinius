#ifndef RBX_HASH_HPP
#define RBX_HASH_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {

  class LookupTable;
  class Tuple;

  #define HASH_MINSIZE 16
  class Hash : public Object {
    public:
    const static size_t fields = 6;
    const static object_type type = HashType;

    OBJECT keys; // slot
    Tuple* values; // slot
    FIXNUM bins; // slot
    FIXNUM entries; // slot
    OBJECT default_value; // slot
    OBJECT default_proc; // slot

    static Hash* create(STATE, size_t size = HASH_MINSIZE);
    void   setup(STATE, size_t size);
    Hash*  dup(STATE);
    static Tuple* entry_new(STATE, hashval hsh, OBJECT key, OBJECT data);
    static Tuple* entry_append(STATE, Tuple* top, Tuple* nxt);
    OBJECT add_entry(STATE, hashval hsh, Tuple* ent);
    void   redistribute(STATE);
    Tuple* find_entry(STATE, hashval hsh);
    OBJECT add(STATE, hashval hsh, OBJECT key, OBJECT data);
    OBJECT set(STATE, OBJECT key, OBJECT val);
    OBJECT get(STATE, hashval hsh);
    bool    lookup(STATE, OBJECT key, hashval hash, OBJECT *value);
    bool    lookup2(STATE, bool (*compare)(STATE, OBJECT, OBJECT),
        OBJECT key, hashval hash, OBJECT *value);
    void   assign(STATE, bool (*compare)(STATE, OBJECT, OBJECT),
        OBJECT key, hashval hash, OBJECT value);
    OBJECT get_undef(STATE, hashval hsh);
    OBJECT remove(STATE, hashval hsh);

    static Hash*  from_tuple(STATE, Tuple* tup);
    static Tuple* csm_new(STATE);
    static OBJECT csm_find(STATE, Tuple* csm, OBJECT key);
    static OBJECT csm_add(STATE, Tuple* csm, OBJECT key, OBJECT val);
    static Hash*  csm_into_hash(STATE, Tuple* csm);
    static LookupTable* csm_into_lookuptable(STATE, Tuple* csm);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

#define HASH_MAX_DENSITY 0.75
#define hash_redistribute_p(hash) (hash->entries->n2i() >= HASH_MAX_DENSITY * hash->values->field_count)
};

#endif
