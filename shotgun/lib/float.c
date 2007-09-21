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

OBJECT float_from_string(STATE, char *str) {
  double d;
  
  sscanf(str, "%lf", &d);
  return float_new(state, d);
}

void float_into_string(STATE, OBJECT self, char *buf, int sz) {
  snprintf(buf, sz, "%+.17e", FLOAT_TO_DOUBLE(self));
}

OBJECT float_sprintf(STATE, OBJECT fmt, OBJECT val) {
 char buf[32];
 char *p;

 p = string_byte_address(state, fmt);
 snprintf(buf, 32, p, FLOAT_TO_DOUBLE(val));
 return string_new(state, buf);
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

int float_to_i(double value) {
  if (value > 0.0) value = floor(value);
  if (value < 0.0) value = ceil(value);
  return (int) value;
}

int float_round(double value) {
  if (value > 0.0) value = floor(value+0.5);
  if (value < 0.0) value = ceil(value-0.5);
  return (int)value;
}
