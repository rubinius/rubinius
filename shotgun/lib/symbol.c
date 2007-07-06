#include "shotgun.h"
#include "tuple.h"
#include "hash.h"
#include "string.h"
#include "symbol.h"

#define StartSize 256
#define Increments 32

OBJECT symbol_from_index(STATE, int idx) {
  return APPLY_TAG(idx, TAG_SYMBOL);
}

int symbol_to_index(STATE, OBJECT self) {
  return STRIP_TAG((int)self);
}

OBJECT symtbl_new(STATE) {
  OBJECT tbl;
  tbl = symtbl_allocate(state);
  symtbl_set_symbols(tbl, tuple_new(state, StartSize));
  symtbl_set_strings(tbl, hash_new_sized(state, StartSize));
  return tbl;
}

OBJECT symtbl_lookup_cstr(STATE, OBJECT self, const char *str) {
  return symtbl_lookup_str_with_size(state, self, str, strlen(str));
}

OBJECT symtbl_lookup_str_with_size(STATE, OBJECT self,
                                   const char *str, int size) {
  unsigned int hash;
  OBJECT strs, idx, syms;

  hash = string_hash_str_with_size(state, str, size);
  strs = symtbl_get_strings(self);
  syms = symtbl_get_symbols(self);
  
  idx = hash_get(state, strs, hash);
  
  /* If it wasn't present, use the longer, more correct version. */
  if(NIL_P(idx) || idx == Qundef) {
    return symtbl_lookup(state, self, string_new2(state, str, size));
  }
  
  return symbol_from_index(state, FIXNUM_TO_INT(idx));
}

OBJECT symtbl_lookup(STATE, OBJECT self, OBJECT string) {
  unsigned int hash, sz;
  OBJECT idx, strs, syms, ns, obj;
  
  hash = string_hash_int(state, string);
  strs = symtbl_get_strings(self);
  syms = symtbl_get_symbols(self);
  
  idx = hash_get(state, strs, hash);
  // printf("Looking up symbol: %od, %x\n", hash, idx);
  
  if(NIL_P(idx) || idx == Qundef) {
    idx = hash_get_entries(strs);
    sz = tuple_fields(state, syms);
    if(FIXNUM_TO_INT(idx) == sz) {
      ns = tuple_new(state, sz + Increments);
      object_copy_fields_from(state, syms, ns, 0, sz);
      symtbl_set_symbols(self, ns);
      syms = ns;
    }
    
    tuple_put(state, syms, FIXNUM_TO_INT(idx), string);
    // printf("Adding to symbol table: %d, %s\n", FIXNUM_TO_INT(idx), string_byte_address(state, string));
    hash_add(state, strs, hash, I2N(hash), idx);
  }
  
  obj = symbol_from_index(state, FIXNUM_TO_INT(idx));
  return obj;
}

OBJECT symbol_to_string(STATE, OBJECT self) {
  return symtbl_find_string(state, state->global->symbols, self);
}

OBJECT symtbl_find_string(STATE, OBJECT self, OBJECT sym) {
  int idx;
  OBJECT str;
  
  idx = symbol_to_index(state, sym);
  str = tuple_at(state, symtbl_get_symbols(self), idx);
  return str;
}

