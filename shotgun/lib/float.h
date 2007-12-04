#ifndef RBS_FLOAT_H
#define RBS_FLOAT_H

OBJECT float_new(STATE, double dbl);
OBJECT float_from_string(STATE, char *str);
void float_into_string(STATE, OBJECT self, char *buf, int sz);
double float_add(double a, double b);
double float_sub(double a, double b);
double float_mul(double a, double b);
double float_div(double a, double b);
double float_uminus(double a);
int float_equal(double a, double b);
int float_compare(double a, double b);
int float_to_i(double value);
int float_round(double value);

#endif
