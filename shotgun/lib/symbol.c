#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/symbol.h"

#define StartSize 256
#define Increments 32

/*
 *
 * The case that prompted code the symbol table to check the key value.
 * str:  __uint_fast64_t
 * hash: 112644932
 *
 * str:  TkIF_MOD
 * hash: 112644932
 */ 

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
  OBJECT strs, idx, syms, ent;

  hash = string_hash_str_with_size(state, str, size);
  strs = symtbl_get_strings(self);
  syms = symtbl_get_symbols(self);
  
  ent = hash_find_entry(state, strs, hash);
  
  /* If it wasn't present, use the longer, more correct version. */
  if(NIL_P(ent) || ent == Qundef) {
    return symtbl_lookup(state, self, string_new2(state, str, size));
  } else {
    OBJECT key = tuple_at(state, ent, 1);
    char *cur = string_byte_address(state, key);

    /* Check that this is actually the right string. */
    if(size != N2I(string_get_bytes(key)) || strncmp(cur, str, size)) {
      return symtbl_lookup(state, self, string_new2(state, str, size));
    }

    idx = tuple_at(state, ent, 2);
  }
  
  return symbol_from_index(state, N2I(idx));
}

OBJECT symtbl_lookup(STATE, OBJECT self, OBJECT string) {
  unsigned int hash, sz;
  OBJECT idx, strs, syms, ns, obj;
  
  hash = string_hash_int(state, string);
  strs = symtbl_get_strings(self);
  syms = symtbl_get_symbols(self);
  
 
  idx = Qnil;

  if(!hash_lookup2(state, string_equal_p, strs, string, hash, &idx)) {
    idx = hash_get_entries(strs);
    sz = tuple_fields(state, syms);
    if(N2I(idx) == sz) {
      ns = tuple_new(state, sz + Increments);
      object_copy_fields_from(state, syms, ns, 0, sz);
      symtbl_set_symbols(self, ns);
      syms = ns;
    }

    tuple_put(state, syms, N2I(idx), string);
    hash_assign(state, string_equal_p, strs, string, hash, idx);
  }

  obj = symbol_from_index(state, N2I(idx));
  return obj;
}

OBJECT symbol_to_string(STATE, OBJECT self) {
  return symtbl_find_string(state, state->global->symbols, self);
}

OBJECT symtbl_find_string(STATE, OBJECT self, OBJECT sym) {
  size_t idx;
  OBJECT str;
  
  idx = symbol_to_index(state, sym);
  str = tuple_at(state, symtbl_get_symbols(self), idx);
  return str;
}

