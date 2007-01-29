#include "shotgun.h"
#include "string.h"
#include "bignum.h"

#include <ctype.h>
#include <string.h>
#include <math.h>

// copied from ruby 1.8.x source, ruby.h
/* need to include <ctype.h> to use these macros */
#ifndef ISPRINT
#define ISASCII(c) isascii((int)(unsigned char)(c))
#undef ISPRINT
#define ISPRINT(c) (ISASCII(c) && isprint((int)(unsigned char)(c)))
#define ISSPACE(c) (ISASCII(c) && isspace((int)(unsigned char)(c)))
#define ISUPPER(c) (ISASCII(c) && isupper((int)(unsigned char)(c)))
#define ISLOWER(c) (ISASCII(c) && islower((int)(unsigned char)(c)))
#define ISALNUM(c) (ISASCII(c) && isalnum((int)(unsigned char)(c)))
#define ISALPHA(c) (ISASCII(c) && isalpha((int)(unsigned char)(c)))
#define ISDIGIT(c) (ISASCII(c) && isdigit((int)(unsigned char)(c)))
#define ISXDIGIT(c) (ISASCII(c) && isxdigit((int)(unsigned char)(c)))
#endif

OBJECT float_new(STATE, double dbl) {
  double *value;
  OBJECT o;
  o = object_memory_new_opaque(state, BASIC_CLASS(floatpoint), sizeof(double));
  value = (double *)BYTES_OF(o);
  *value = dbl;
  return o;
}

OBJECT float_add(STATE, OBJECT a, OBJECT b) {
  return float_new(state, FLOAT_TO_DOUBLE(a) + FLOAT_TO_DOUBLE(b));
}

OBJECT float_sub(STATE, OBJECT a, OBJECT b) {
  return float_new(state, FLOAT_TO_DOUBLE(a) - FLOAT_TO_DOUBLE(b));
}

OBJECT float_mul(STATE, OBJECT a, OBJECT b) {
  return float_new(state, FLOAT_TO_DOUBLE(a) * FLOAT_TO_DOUBLE(b));
}

OBJECT float_div(STATE, OBJECT a, OBJECT b) {
  return float_new(state, FLOAT_TO_DOUBLE(a) / FLOAT_TO_DOUBLE(b));
}

OBJECT float_uminus(STATE, OBJECT self) {
  return float_new(state, -FLOAT_TO_DOUBLE(self));
}

OBJECT float_equal(STATE, OBJECT a, OBJECT b) {
  return FLOAT_TO_DOUBLE(a) == FLOAT_TO_DOUBLE(b) ? Qtrue : Qfalse;
}

OBJECT float_pow(STATE, OBJECT a, OBJECT b) {
  return float_new(state, pow(FLOAT_TO_DOUBLE(a), FLOAT_TO_DOUBLE(b)));
}

OBJECT float_to_i(STATE, OBJECT self) {
  double value;
  value = FLOAT_TO_DOUBLE(self);
  if (value > 0.0) value = floor(value);
  if (value < 0.0) value = ceil(value);
  
  return bignum_normalize(state, bignum_from_double(state, value));
}

OBJECT float_to_s(STATE, OBJECT self) {
  char buf[32];
  double value;
  char *p, *e;
  
  value = FLOAT_TO_DOUBLE(self);
  
  // adapted from ruby 1.8.x source
  if (isinf(value))
	  return string_new(state, value < 0 ? "-Infinity" : "Infinity");
  else if(isnan(value))
	  return string_new(state, "NaN");

  snprintf(buf, 32, "%#.15g", value); /* ensure to print decimal point */
  if (!(e = strchr(buf, 'e'))) {
	  e = buf + strlen(buf);
  }
  if (!ISDIGIT(e[-1])) { /* reformat if ended with decimal point (ex 111111111111111.) */
	  snprintf(buf, 32, "%#.14e", value);
  	if (!(e = strchr(buf, 'e'))) {
  	  e = buf + strlen(buf);
  	}
  }
  p = e;
  while (p[-1]=='0' && ISDIGIT(p[-2]))
	p--;
	memmove(p, e, strlen(e)+1);
  return string_new(state, buf);
}

void float_into_string(STATE, OBJECT self, char *buf, int sz) {
  snprintf(buf, sz, "%+.17e", FLOAT_TO_DOUBLE(self));
}

OBJECT float_from_string(STATE, char *str) {
  double d;
  
  sscanf(str, "%lf", &d);
  return float_new(state, d);
}

inline OBJECT float_nan_p(STATE, OBJECT self) {
  return isnan(FLOAT_TO_DOUBLE(self)) ? Qtrue : Qfalse;
}

inline OBJECT float_infinite_p(STATE, OBJECT self) {
  double value = FLOAT_TO_DOUBLE(self);
  
  if (isinf(value))
    return I2N(value < 0 ? -1 : 1);
    
  return Qnil;
}
