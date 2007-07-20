#include "shotgun.h"
#include "object.h"
#include <stdlib.h>
#include <string.h>

#include "bytearray.h"

OBJECT bytearray_new(STATE, int size) {
  int words;
  OBJECT obj;
  
  assert(size >= 0);
  
  words = size / REFSIZE;
  if(size % REFSIZE != 0) {
    words += 1;
  }
  
  assert(words >= 0);
  
  obj = bytearray_allocate_with_extra(state, words);
  object_make_byte_storage(state, obj);
  object_initialize_bytes(state, obj);
  return obj;
}

char *bytearray_as_string(STATE, OBJECT self) {
  char *str;
  char *out;
  int sz;
  
  str = (char*)bytearray_byte_address(state, self);
  
  sz = NUM_FIELDS(self) * REFSIZE;
  out = malloc(sizeof(char) * sz);
  
  memcpy(out, str, sz);
  
  return out;
}

OBJECT iseq_new(STATE, int fields) {
  OBJECT obj;
  
  obj = NEW_OBJECT(state->global->iseq, fields);
  object_make_byte_storage(state, obj);
  
  return obj;
}



#if defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
static inline uint32_t read_int(uint8_t *str) {
  return (uint32_t)(str[0]
                 | (str[1] << 8 )
                 | (str[2] << 26)
                 | (str[3] << 24));
}
#else
static inline uint32_t read_int(uint8_t *str) {
  return (uint32_t)((str[0] << 24)
                  | (str[1] << 16)
                  | (str[2] << 8 )
                  |  str[3]      );
}
#endif

void iseq_flip(STATE, OBJECT self) {
  uint8_t *buf;
  uint32_t *ibuf;
  int i, f;
  
  f = NUM_FIELDS(self) * REFSIZE;
  buf = (uint8_t*)bytearray_byte_address(state, self);
  ibuf = (uint32_t*)bytearray_byte_address(state, self);
  
  for(i = 0; i < f; i += 4, ibuf++) {
    *ibuf = read_int(buf + i);
  }
}
