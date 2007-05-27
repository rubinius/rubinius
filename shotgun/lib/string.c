#include "shotgun.h"
#include "bytearray.h"
#include "object.h"
#include "symbol.h"
#include "string.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int debugging = 0;

#define STRING_P(obj) (REFERENCE_P(obj) && HEADER(obj)->klass == state->global->string)

OBJECT string_new2(STATE, const char *str, int sz) {
  OBJECT obj, data;
  char *ba;
  
  assert(sz >= 0);
  obj = string_allocate(state);
  string_set_bytes(obj, I2N(sz));
  string_set_characters(obj, I2N(sz));
  string_set_encoding(obj, Qnil);

  data = bytearray_new(state, sz+1);
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

OBJECT string_new(STATE, const char *str) {
  int sz;  
  sz = strlen(str);
  return string_new2(state, str, sz);
}

OBJECT string_dup(STATE, OBJECT self) {
  char *ba;
  
  assert(STRING_P(self));
  ba = bytearray_byte_address(state, string_get_data(self));
  
  return string_new2(state, ba, FIXNUM_TO_INT(string_get_bytes(self)));
}

OBJECT string_append(STATE, OBJECT self, OBJECT other) {
  OBJECT cur, obs, nd;
  int cur_sz, oth_sz, ns, tmp;
  char *ba;
  
  assert(STRING_P(self));
  assert(STRING_P(other));
  
  cur = string_get_data(self);
  obs = string_get_data(other);
  cur_sz = FIXNUM_TO_INT(string_get_bytes(self));
  oth_sz = FIXNUM_TO_INT(string_get_bytes(other));
  
  ns = cur_sz + oth_sz;
  tmp = bytearray_bytes(state, cur);
  if(ns+1 > tmp) {
    nd = bytearray_new(state, ns+1);
    object_copy_bytes_into(state, cur, nd, cur_sz, 0);
    object_copy_bytes_into(state, obs, nd, oth_sz, cur_sz);
    ba = bytearray_byte_address(state, nd);
    string_set_data(self, nd);
  } else {
    object_copy_bytes_into(state, obs, cur, oth_sz, cur_sz);
    ba = bytearray_byte_address(state, cur);
  }
  ba[ns] = 0;
  string_set_bytes(self, I2N(ns));
  return self;
}

char *string_as_string(STATE, OBJECT self) {
  int i;
  OBJECT data;
  char *out;
  
  assert(STRING_P(self) || debugging);
  i = FIXNUM_TO_INT(string_get_bytes(self));
  data = string_get_data(self);
  
  out = (char*)malloc(i+1);
  
  if(!NIL_P(data)) {
    memcpy(out, bytearray_byte_address(state, data), i);
    out[i] = 0;
  } else {
    out[0] = 0;
  }
  return out;
}

char *string_byte_address(STATE, OBJECT self) {
  OBJECT data;

  assert(STRING_P(self));
  data = string_get_data(self);
  return bytearray_byte_address(state, data);
}

double string_to_double(STATE, OBJECT self) {
  OBJECT data, str;
  double value;
  char *p, *n, *ba, *rest;
  
  assert(STRING_P(self));
  str = string_dup(state, self);
  data = string_get_data(str);
  ba = bytearray_byte_address(state, data);
  
  p = ba;
  while (ISSPACE(*p)) p++;
  n = p;
  while (*p) {
    if (*p == '_') {
      p++;
    } else {
      *n++ = *p++;
    }
  }
  *n = 0;
  
  value = strtod(ba, &rest);
  if (errno == ERANGE) {
	  printf("Float %s out of range\n", ba);
  }
  
  return value;
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
  unsigned int sz, h;
  OBJECT data;
  
  assert(STRING_P(self));
  data = string_get_data(self);
  if(HEADER(data)->hash != 0) {
    return HEADER(data)->hash;
  }
  bp = (unsigned char*)bytearray_byte_address(state, data);
  sz = FIXNUM_TO_INT(string_get_bytes(self));
  
  h = _hash_str(bp, sz);
  HEADER(data)->hash = h;
  return h;
}

unsigned int string_hash_cstr(STATE, const char *bp) {
  unsigned int sz = strlen(bp);
  return _hash_str((unsigned char*)bp, sz);
}

OBJECT string_to_sym(STATE, OBJECT self) {
  assert(STRING_P(self));
  return symtbl_lookup(state, state->global->symbols, self);
}
