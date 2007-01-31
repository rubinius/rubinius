#ifndef RBS_FIXNUM
#define RBS_FIXNUM 1

OBJECT fixnum_add(STATE, OBJECT a, OBJECT b);
OBJECT fixnum_sub(STATE, OBJECT a, OBJECT b);
OBJECT fixnum_mul(STATE, OBJECT a, OBJECT b);
OBJECT fixnum_div(STATE, OBJECT a, OBJECT b);
OBJECT fixnum_divmod(STATE, OBJECT a, OBJECT b);

#endif

