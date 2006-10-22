#include "rubinius.h"
#include "hash.h"

OBJECT methtbl_new(STATE) {
  OBJECT obj;
  
  obj = methtbl_allocate(state);
  hash_setup(state, obj);
  return obj;
}