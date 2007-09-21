#include "shotgun.h"
#include "string.h"
#include "bignum.h"
#include "array.h"

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

double float_add(double a, double b) {
  return a + b;
}

double float_sub(double a, double b) {
  return a - b;
}

double float_mul(double a, double b) {
  return a * b;
}

double float_div(double a, double b) {
  return a / b;
}

double float_uminus(double a) {
  return -a;
}

int float_equal(double a, double b) {
  return a == b;
}

int float_compare(double a, double b) {
  if (a < b)
    return -1;
  else if (a > b)
    return 1;
  return 0;
}

// OBJECT float_pow(STATE, OBJECT a, OBJECT b) {
//   return float_new(state, pow(FLOAT_TO_DOUBLE(a), FLOAT_TO_DOUBLE(b)));
// }

// OBJECT float_to_i(STATE, OBJECT self) {
//   double value;
//   value = FLOAT_TO_DOUBLE(self);
//   if (value > 0.0) value = floor(value);
//   if (value < 0.0) value = ceil(value);
//   
//   return bignum_from_double(state, value);
// }
// 
int float_to_i(double value) {
  if (value > 0.0) value = floor(value);
  if (value < 0.0) value = ceil(value);

  return (int) value;
}

// OBJECT float_to_s(STATE, OBJECT self) {
//   char buf[32];
//   double value;
//   char *p, *e;
//   
//   value = FLOAT_TO_DOUBLE(self);
//   
//   // adapted from ruby 1.8.x source
//   if(isinf(value)) {
//    return string_new(state, value < 0 ? "-Infinity" : "Infinity");
//   } else if(isnan(value)) {
//    return string_new(state, "NaN");
//  }
// 
//   snprintf(buf, 32, "%#.15g", value); /* ensure to print decimal point */
//   if (!(e = strchr(buf, 'e'))) {
//    e = buf + strlen(buf);
//   }
//   if (!ISDIGIT(e[-1])) { /* reformat if ended with decimal point (ex 111111111111111.) */
//    snprintf(buf, 32, "%#.14e", value);
//    if (!(e = strchr(buf, 'e'))) {
//      e = buf + strlen(buf);
//    }
//   }
//   p = e;
//   while (p[-1]=='0' && ISDIGIT(p[-2]))
//  p--;
//  memmove(p, e, strlen(e)+1);
//   return string_new(state, buf);
// }

void float_into_string(STATE, OBJECT self, char *buf, int sz) {
  snprintf(buf, sz, "%+.17e", FLOAT_TO_DOUBLE(self));
}

char *float_sprintf(char *buf, int size, char *fmt, double value) {
  snprintf(buf, size, fmt, value);
  return buf;
}

// OBJECT float_sprintf(STATE, OBJECT fmt, OBJECT val) {
//  char buf[32];
//  char *p;
// 
//  p = string_byte_address(state, fmt);
//  snprintf(buf, 32, p, FLOAT_TO_DOUBLE(val));
//  return string_new(state, buf);
// }
// 
OBJECT float_from_string(STATE, char *str) {
  double d;
  
  sscanf(str, "%lf", &d);
  return float_new(state, d);
}

// inline OBJECT float_nan_p(STATE, OBJECT self) {
//   return isnan(FLOAT_TO_DOUBLE(self)) ? Qtrue : Qfalse;
// }
// 
// inline OBJECT float_infinite_p(STATE, OBJECT self) {
//   double value = FLOAT_TO_DOUBLE(self);
//   
//   if (isinf(value))
//     return I2N(value < 0 ? -1 : 1);
//     
//   return Qnil;
// }

// OBJECT float_divmod(STATE, OBJECT a, OBJECT b) {
//   OBJECT ary;
//   double x, y, div, mod;
//   
//   x = FLOAT_TO_DOUBLE(a);
//   y = FLOAT_TO_DOUBLE(b);
//   mod = fmod(x, y);
//   div = (x - mod) / y;
//   if (y*mod < 0) {
//    mod += y;
//    div -= 1.0;
//   }
//   ary = array_new(state, 2);
//   array_set(state, ary, 0, I2N(div));
//   array_set(state, ary, 1, float_new(state, mod));
//   return ary;
// }

int float_round(double value) {
  if (value > 0.0) value = floor(value+0.5);
  if (value < 0.0) value = ceil(value-0.5);
  return (int)value;
}

// OBJECT float_round(STATE, OBJECT self) {
//   double value;
//   
//   value = FLOAT_TO_DOUBLE(self);
//   if (value > 0.0) value = floor(value+0.5);
//   if (value < 0.0) value = ceil(value-0.5);
//   return bignum_from_double(state, value);
// }
