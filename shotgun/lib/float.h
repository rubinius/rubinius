#ifndef RBS_FLOAT_H
#define RBS_FLOAT_H

#include <math.h>

/* use IEEE 64bit values if not defined */
#ifndef FLT_RADIX
#define FLT_RADIX 2
#endif
#ifndef FLT_ROUNDS
#define FLT_ROUNDS 1
#endif
#ifndef DBL_MIN
#define DBL_MIN 2.2250738585072014e-308
#endif
#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623157e+308
#endif
#ifndef DBL_MIN_EXP
#define DBL_MIN_EXP (-1021)
#endif
#ifndef DBL_MAX_EXP
#define DBL_MAX_EXP 1024
#endif
#ifndef DBL_MIN_10_EXP
#define DBL_MIN_10_EXP (-307)
#endif
#ifndef DBL_MAX_10_EXP
#define DBL_MAX_10_EXP 308
#endif
#ifndef DBL_DIG
#define DBL_DIG 15
#endif
#ifndef DBL_MANT_DIG
#define DBL_MANT_DIG 53
#endif
#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131e-16
#endif
/* End borrowing from MRI 1.8.6 stable */

OBJECT float_new(STATE, double dbl);
OBJECT float_from_string(STATE, char *str);
void float_into_string(STATE, OBJECT self, char *buf, int sz);
OBJECT float_coerce(STATE, OBJECT value);
OBJECT float_to_i_prim(STATE, double value);
OBJECT float_compare_prim(STATE, double a, double b);
int float_to_i(double value);

static inline int float_bounded_p(double value) {
  if(value <= (double)FIXNUM_MAX && value >= (double)FIXNUM_MIN) {
    return 1;
  }
  return 0;
}

static inline double float_truncate(double value) {
  if(value > 0.0) {
    return floor(value);
  } else if(value < 0.0) {
    return ceil(value);
  }
  return value;
}

#endif
