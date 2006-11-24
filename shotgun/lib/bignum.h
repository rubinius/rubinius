#ifndef RBS_BIGNUM
#define RBS_BIGNUM 1

OBJECT bignum_new(STATE, int num);
OBJECT bignum_normalize(STATE, OBJECT b);
OBJECT bignum_add(STATE, OBJECT a, OBJECT b);
OBJECT bignum_sub(STATE, OBJECT a, OBJECT b);
OBJECT bignum_mul(STATE, OBJECT a, OBJECT b);
OBJECT bignum_equal(STATE, OBJECT a, OBJECT b);
OBJECT bignum_compare(STATE, OBJECT a, OBJECT b);
OBJECT bignum_to_s(STATE, OBJECT self, OBJECT radix);
void bignum_into_string(STATE, OBJECT self, int radix, char *buf, int sz);

#endif