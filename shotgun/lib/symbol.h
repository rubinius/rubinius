OBJECT symbol_from_index(STATE, int idx);
int symbol_to_index(STATE, OBJECT self);
OBJECT symtbl_new(STATE);
OBJECT symtbl_lookup(STATE, OBJECT self, OBJECT string);
OBJECT symtbl_find_string(STATE, OBJECT self, OBJECT sym);
OBJECT symbol_to_string(STATE, OBJECT self);

