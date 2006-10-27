OBJECT string_new(STATE, char *str);
OBJECT string_new2(STATE, char *str, int sz);
OBJECT string_dup(STATE, OBJECT self);
OBJECT string_append(STATE, OBJECT self, OBJECT other);
char *string_as_string(STATE, OBJECT self);
char *string_byte_address(STATE, OBJECT self);
int string_hash_int(STATE, OBJECT self);
OBJECT string_to_sym(STATE, OBJECT self);