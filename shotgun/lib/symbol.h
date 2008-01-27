#ifndef RBS_SYMBOL_H
#define RBS_SYMBOL_H 1

OBJECT symtbl_new(STATE);
OBJECT symtbl_lookup(STATE, OBJECT self, OBJECT string);
OBJECT symtbl_lookup_cstr(STATE, OBJECT self, const char *str);
OBJECT symtbl_lookup_str_with_size(STATE, OBJECT self, const char *str, int size);
OBJECT symtbl_find_string(STATE, OBJECT self, OBJECT sym);
OBJECT symbol_to_string(STATE, OBJECT self);

#define symbol_from_cstr(st, str) symtbl_lookup_cstr(st, st->global->symbols, str)
#define SYM(str) symtbl_lookup_cstr(state, state->global->symbols, str)

static inline OBJECT symbol_from_index(STATE, size_t idx) {
  return DATA_APPLY_TAG(idx, DATA_TAG_SYMBOL);
}

static inline size_t symbol_to_index(STATE, OBJECT self) {
  return DATA_STRIP_TAG(self);
}

#endif
