#ifndef RBS_FLOAT
#define RBS_FLOAT 1

OBJECT float_new(STATE, double dbl);
OBJECT float_add(STATE, OBJECT a, OBJECT b);
OBJECT float_sub(STATE, OBJECT a, OBJECT b);
OBJECT float_mul(STATE, OBJECT a, OBJECT b);
OBJECT float_equal(STATE, OBJECT a, OBJECT b);
OBJECT float_compare(STATE, OBJECT a, OBJECT b);
OBJECT float_to_s(STATE, OBJECT self);
void float_into_string(STATE, OBJECT self, char *buf, int sz);
OBJECT float_from_string(STATE, char *str);

#endif