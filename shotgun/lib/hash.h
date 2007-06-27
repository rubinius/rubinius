OBJECT hash_new(STATE);
OBJECT hash_new_sized(STATE, int size);
void hash_setup(STATE, OBJECT hsh, int size);
OBJECT hash_add(STATE, OBJECT h, unsigned int hsh, OBJECT key, OBJECT data);
OBJECT hash_set(STATE, OBJECT hash, OBJECT key, OBJECT val);
OBJECT hash_get(STATE, OBJECT hash, unsigned int hsh);
OBJECT hash_get_undef(STATE, OBJECT hash, unsigned int hsh);
OBJECT hash_delete(STATE, OBJECT self, unsigned int hsh);
OBJECT hash_s_from_tuple(STATE, OBJECT tup);
OBJECT hash_get_undef(STATE, OBJECT hash, unsigned int hsh);
OBJECT hash_find_entry(STATE, OBJECT h, unsigned int hsh);

#define hash_find(state, hash, key) (hash_get(state, hash, object_hash_int(state, key)))

#define hash_find_undef(state, hash, key) (hash_get_undef(state, hash, object_hash_int(state, key)))

#define CSM_SIZE 6

#define csm_new(st) tuple_new(st, CSM_SIZE)
#define csm_size(st, obj) (NUM_FIELDS(obj) / 2)

OBJECT csm_find(STATE, OBJECT csm, OBJECT key);
OBJECT csm_add(STATE, OBJECT csm, OBJECT key, OBJECT val);
OBJECT csm_into_hash(STATE, OBJECT csm);

