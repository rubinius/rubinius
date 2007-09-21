#ifndef RBS_FLOAT
#define RBS_FLOAT 1

OBJECT float_new(STATE, double dbl);
double float_add(double a, double b);
double float_sub(double a, double b);
double float_mul(double a, double b);
double float_div(double a, double b);
double float_uminus(double a);
int float_equal(double a, double b);
int float_compare(double a, double b);
// OBJECT float_to_s(STATE, OBJECT self);
void float_into_string(STATE, OBJECT self, char *buf, int sz);
OBJECT float_from_string(STATE, char *str);
// inline OBJECT float_nan_p(STATE, OBJECT self);
// inline OBJECT float_infinite_p(STATE, OBJECT self);
// OBJECT float_pow(STATE, OBJECT a, OBJECT b);
// OBJECT float_to_i(STATE, OBJECT self);
int float_to_i(double value);
// OBJECT float_divmod(STATE, OBJECT a, OBJECT b);
// OBJECT float_round(STATE, OBJECT self);
// OBJECT float_sprintf(STATE, OBJECT fmt, OBJECT val); 
char *float_sprintf(char *buf, int size, char *fmt, double value);
#endif
