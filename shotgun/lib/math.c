#include "shotgun.h"
#include "float.h"

#include <string.h>
#include <math.h>


OBJECT math_sqrt(STATE, OBJECT self, OBJECT a) {
  return float_new(state, sqrt(FLOAT_TO_DOUBLE(a)) );
}

