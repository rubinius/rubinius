#include <string.h>
#include <math.h>

/* Begin borrowing from MRI 1.8.6 stable */
#if defined(__FreeBSD__) && __FreeBSD__ < 4
#include <floatingpoint.h>
#endif

#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#ifdef WORDS_BIGENDIAN
#define word0(x) ((unsigned int *)&x)[0]
#define word1(x) ((unsigned int *)&x)[1]
#else
#define word0(x) ((unsigned int *)&x)[1]
#define word1(x) ((unsigned int *)&x)[0]
#endif     

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/array.h"
#include "shotgun/lib/bignum.h"
#include "shotgun/lib/float.h"

int float_radix()      { return FLT_RADIX; }
int float_rounds()     { return FLT_ROUNDS; }
double float_min()     { return DBL_MIN; }
double float_max()     { return DBL_MAX; }
int float_min_exp()    { return DBL_MIN_EXP; }
int float_max_exp()    { return DBL_MAX_EXP; }
int float_min_10_exp() { return DBL_MIN_10_EXP; }
int float_max_10_exp() { return DBL_MAX_10_EXP; }
int float_dig()        { return DBL_DIG; }
int float_mant_dig()   { return DBL_MANT_DIG; }
double float_epsilon() { return DBL_EPSILON; }

OBJECT float_new(STATE, double dbl) {
  double *value;
  OBJECT o;
  o = object_memory_new_opaque(state, BASIC_CLASS(floatpoint), sizeof(double));
  value = (double *)BYTES_OF(o);
  *value = dbl;
  return o;
}

/* This functions is only used when unmarshalling. 
 * The assumptions made here are therefore safe.
 * String#to_f uses string_to_double
 */
OBJECT float_from_string(STATE, char *str) {
  char *endp;
  double d;  
  d = strtod(str, &endp);
  if (str != endp && *endp == '\0') {
    return float_new(state, d);
  }
  /* When we get here, we might have a system that doesn't conform to
     C99 (OpenBSD is at least one) that can't handle Infinity / NaN.
     We test the strings here manually and fix it if needed. */
  
  int sign = 0;
	
  if (*str == '-') {
    sign = 1;
    str++;
  } else if (*str == '+') {
    str++;
  }
  
  if (*str == 'I' || *str == 'i') {
    return float_new(state, sign ? -HUGE_VAL : HUGE_VAL);
  } 
  if (*str == 'N' || *str == 'n') {
    word0(d) = 0x7ff80000;
    word1(d) = 0;
    return float_new(state, d);
  }
  
  return Qnil;
}

void float_into_string(STATE, OBJECT self, char *buf, int sz) {
  snprintf(buf, sz, "%+.17e", FLOAT_TO_DOUBLE(self));
}

OBJECT float_compare(STATE, double a, double b) {
  if(a < b) {
    return I2N(-1);
  } else if(a > b) {
    return I2N(1);
  }
  return I2N(0);
}

OBJECT float_coerce(STATE, OBJECT value) {
  if(FIXNUM_P(value)) {
    return float_new(state, (double)N2I(value));
  } else if(BIGNUM_P(value)) {
    return float_new(state, bignum_to_double(state, value));
  }
  return value;
}
