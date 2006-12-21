#include "shotgun.h"
#include "object.h"
#include <stdlib.h>
#include <string.h>

OBJECT bytearray_new(STATE, int size) {
  int words;
  OBJECT obj;
  
  assert(size >= 0);
  
  words = size / 4;
  if(size % 4 != 0) {
    words += 1;
  }
  
  assert(words >= 0);
  
  obj = bytearray_allocate_with_extra(state, words);
  object_make_byte_storage(state, obj);
  object_initialize_bytes(state, obj);
  return obj;
}

void *bytearray_byte_address(STATE, OBJECT self) {
  void *addr;
  
  addr = (void*)(self + (HEADER_SIZE * 4));
  
  return addr;
}

int bytearray_bytes(STATE, OBJECT self) {
  return SIZE_OF_BODY(self);
}

char *bytearray_as_string(STATE, OBJECT self) {
  char *str;
  char *out;
  int sz;
  
  str = (char*)bytearray_byte_address(state, self);
  
  sz = NUM_FIELDS(self) * 4;
  out = malloc(sizeof(char) * sz);
  
  memcpy(out, str, sz);
  
  return out;
}
