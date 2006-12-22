#include "shotgun.h"
#include "bytearray.h"
#include "object.h"
#include "symbol.h"
#include <string.h>
#include <stdlib.h>

OBJECT string_new2(STATE, char *str, int sz) {
  OBJECT obj, data;
  char *ba;
  
  assert(sz >= 0);
  obj = string_allocate(state);
  string_set_bytes(obj, I2N(sz));
  string_set_characters(obj, I2N(sz));
  string_set_encoding(obj, Qnil);
  
  data = bytearray_new(state, sz);
  ba = bytearray_byte_address(state, data);
  if(str == NULL) {
    memset(ba, 0, sz);
  } else {
    memcpy(ba, str, sz);
    ba[sz] = 0;
  }
  string_set_data(obj, data);
  return obj;
}

OBJECT string_new(STATE, char *str) {
  int sz;
  
  sz = strlen(str);
  return string_new2(state, str, sz);
}

OBJECT string_dup(STATE, OBJECT self) {
  char *ba;
  
  ba = bytearray_byte_address(state, string_get_data(self));
  
  return string_new2(state, ba, FIXNUM_TO_INT(string_get_bytes(self)));
}

OBJECT string_append(STATE, OBJECT self, OBJECT other) {
  OBJECT cur, obs, nd;
  int cur_sz, oth_sz, ns, tmp;
  
  cur = string_get_data(self);
  obs = string_get_data(other);
  cur_sz = FIXNUM_TO_INT(string_get_bytes(self));
  oth_sz = FIXNUM_TO_INT(string_get_bytes(other));
  
  ns = cur_sz + oth_sz;
  tmp = bytearray_bytes(state, cur);
  if(ns > tmp) {
    nd = bytearray_new(state, ns);
    object_copy_bytes_into(state, cur, nd, cur_sz, 0);
    object_copy_bytes_into(state, obs, nd, oth_sz, cur_sz);
    string_set_data(self, nd);
  } else {
    object_copy_bytes_into(state, obs, cur, oth_sz, cur_sz);
  }
  
  string_set_bytes(self, I2N(ns));
  cur_sz = FIXNUM_TO_INT(string_get_bytes(self));
  return self;
}

char *string_as_string(STATE, OBJECT self) {
  int i;
  OBJECT data;
  char *out;
  
  i = FIXNUM_TO_INT(string_get_bytes(self));
  data = string_get_data(self);
  
  out = (char*)malloc(i+1);
  
  memcpy(out, bytearray_byte_address(state, data), i);
  out[i] = 0;
  return out;
}

char *string_byte_address(STATE, OBJECT self) {
  OBJECT data;
  data = string_get_data(self);
  return bytearray_byte_address(state, data);
}

#define HashPrime 16777619
#define MASK_28 (((unsigned int)1<<28)-1)

static inline unsigned int _hash_str(unsigned char *bp, unsigned int sz) {
  unsigned char *be;
  unsigned int hv;
  
  be = bp + sz;
  
  hv = 0;
  
  while(bp < be) {  
    hv *= HashPrime;
    hv ^= *bp++;
  }
  hv = (hv>>28) ^ (hv & MASK_28);
    
  return hv;
}

unsigned int string_hash_int(STATE, OBJECT self) {
  unsigned char *bp;
  unsigned int sz;
  
  bp = (unsigned char*)bytearray_byte_address(state, string_get_data(self));
  sz = FIXNUM_TO_INT(string_get_bytes(self));
  
  return _hash_str(bp, sz);
}

unsigned int string_hash_cstr(STATE, char *bp) {
  unsigned int sz = strlen(bp);
  return _hash_str((unsigned char*)bp, sz);
}

OBJECT string_to_sym(STATE, OBJECT self) {
  return symtbl_lookup(state, state->global->symbols, self);
}
