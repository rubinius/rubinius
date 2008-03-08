#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/lookuptable.h"

OBJECT methtbl_new(STATE) {
  OBJECT obj;
  
  obj = methtbl_allocate(state);
  lookuptable_setup(state, obj, 0);
  return obj;
}
