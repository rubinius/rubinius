#ifndef RBX_HASH_HPP
#define RBX_HASH_HPP

namespace rubinius {

  class LookupTable;

  #define HASH_MINSIZE 16
  class Hash : public BuiltinType {
    public:
    const static size_t fields = 7;
    const static object_type type = HashType;

    OBJECT instance_variables;
    OBJECT keys;
    Tuple* values;
    FIXNUM bins;
    FIXNUM entries;
    OBJECT default_value;
    OBJECT default_proc;

    static Hash* create(STATE, size_t size = HASH_MINSIZE);
    void   setup(STATE, size_t size);
    Hash*  dup(STATE);
    static OBJECT entry_new(STATE, hashval hsh, OBJECT key, OBJECT data);
    static OBJECT entry_append(STATE, OBJECT top, OBJECT nxt);
    OBJECT add_entry(STATE, hashval hsh, OBJECT ent);
    void   redistribute(STATE);
    OBJECT find_entry(STATE, hashval hsh);
    OBJECT add(STATE, hashval hsh, OBJECT key, OBJECT data);
    OBJECT set(STATE, OBJECT key, OBJECT val);
    OBJECT get(STATE, hashval hsh);
    int    lookup(STATE, OBJECT key, hashval hash, OBJECT *value);
    int    lookup2(STATE, int (*compare)(STATE, OBJECT, OBJECT),
        OBJECT key, hashval hash, OBJECT *value);
    void   assign(STATE, int (*compare)(STATE, OBJECT, OBJECT),
        OBJECT key, hashval hash, OBJECT value);
    OBJECT get_undef(STATE, hashval hsh);
    OBJECT remove(STATE, hashval hsh);

    static Hash*  from_tuple(STATE, OBJECT tup);
    static Tuple* csm_new(STATE);
    static OBJECT csm_find(STATE, Tuple* csm, OBJECT key);
    static OBJECT csm_add(STATE, Tuple* csm, OBJECT key, OBJECT val);
    static Hash*  csm_into_hash(STATE, Tuple* csm);
    static LookupTable* csm_into_lookuptable(STATE, Tuple* csm);

  };
};

#endif
