#include "shotgun.h"
#include <stdarg.h>
#include "tuple.h"

OBJECT tuple_enlarge(STATE, OBJECT tup, int inc) {
  int sz;
  OBJECT ns;
  sz = NUM_FIELDS(tup);
  ns = tuple_new(state, sz + inc);
  object_copy_fields_from(state, tup, ns, 0, sz);
  return ns;
}

OBJECT tuple_new2(STATE, int n, ...) {
  va_list ar;
  OBJECT tup;
  int i;
  
  tup = tuple_new(state, n);
  
  va_start(ar, n);
  for(i = 0; i < n; i++) {
    tuple_put(state, tup, i, va_arg(ar, OBJECT));
  }
  va_end(ar);
  return tup;
}
