#include "shotgun.h"
#include "object.h"
#include "flags.h"
#include <stdlib.h>
#include <string.h>

#include "bytearray.h"

OBJECT bytearray_new(STATE, int size) {
  int words;
  OBJECT obj;
  
  words = size / REFSIZE;
  if(size % REFSIZE != 0) {
    words += 1;
  }
  
  obj = bytearray_allocate_with_extra(state, words);
  object_make_byte_storage(state, obj);
  fast_memfill32(BYTES_OF(obj), 0, words);
  object_initialize_bytes(state, obj);
  return obj;
}

OBJECT bytearray_new_dirty(STATE, int size) {
  int words;
  OBJECT obj;
    
  words = size / REFSIZE;
  if(size % REFSIZE != 0) {
    words += 1;
  }
    
  obj = object_memory_new_dirty_object(state->om, BASIC_CLASS(bytearray), words);
  object_make_byte_storage(state, obj);
  
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
#define ISET_FLAG(o) 
#else
static inline uint32_t read_int(uint8_t *str) {
  return (uint32_t)((str[0] << 24)
                  | (str[1] << 16)
                  | (str[2] << 8 )
                  |  str[3]      );
}
#define ISET_FLAG(o) FLAG2_SET(o, IsLittleEndianFlag)
#endif

void iseq_flip(STATE, OBJECT self) {
  uint8_t *buf;
  uint32_t *ibuf;
  int i, f;
  
  f = NUM_FIELDS(self) * REFSIZE;
  buf = (uint8_t*)bytearray_byte_address(state, self);
  ibuf = (uint32_t*)bytearray_byte_address(state, self);
  /* A sanity check. The first thing is always an instruction,
   * and we've got less that 512 instructions, so if it's less
   * it's already been flipped. */
  if(*ibuf < 512) {
    ISET_FLAG(self);
    return;
  }  
  for(i = 0; i < f; i += 4, ibuf++) {
    *ibuf = read_int(buf + i);
  }
  ISET_FLAG(self);
}
