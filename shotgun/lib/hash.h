OBJECT hash_new(STATE);
void hash_setup(STATE, OBJECT hsh);
OBJECT hash_add(STATE, OBJECT h, unsigned int hsh, OBJECT key, OBJECT data);
OBJECT hash_set(STATE, OBJECT hash, OBJECT key, OBJECT val);
OBJECT hash_get(STATE, OBJECT hash, unsigned int hsh);
OBJECT hash_get_undef(STATE, OBJECT hash, unsigned int hsh);
OBJECT hash_find(STATE, OBJECT hash, OBJECT key);
OBJECT hash_delete(STATE, OBJECT self, unsigned int hsh);
OBJECT hash_s_from_tuple(STATE, OBJECT tup);
OBJECT hash_get_undef(STATE, OBJECT hash, unsigned int hsh);

