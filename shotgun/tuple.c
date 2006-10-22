#include "rubinius.h"

OBJECT tuple_new(STATE, int size) {
  return tuple_allocate_with_extra(state, size);
}