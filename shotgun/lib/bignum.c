#include "shotgun.h"
#include "tommath.h"
#include "string.h"
#include <ctype.h>
#include <math.h>

#define NMP mp_int *n; OBJECT n_obj; \
  NEW_STRUCT(n_obj, n, BASIC_CLASS(bignum), mp_int); \
  mp_init(n)
#define MMP mp_int *m; OBJECT m_obj; \
  NEW_STRUCT(m_obj, m, BASIC_CLASS(bignum), mp_int); \
  mp_init(m)


#define MP(k) DATA_STRUCT(k, mp_int*)
#define BDIGIT_DBL long long
#define DIGIT_RADIX (1 << DIGIT_BIT)

void bignum_cleanup(STATE, OBJECT obj) {
  mp_int *n = MP(obj);
  mp_clear(n);
}

void bignum_init(STATE) {
  state_add_cleanup(state, state->global->bignum, bignum_cleanup);
}

static void twos_complement(mp_int *a)
{
  long i = a->used;
  BDIGIT_DBL num;

  while (i--) {
    DIGIT(a,i) = (~DIGIT(a,i)) & (DIGIT_RADIX-1);
  }

  i = 0; num = 1;
  do {
    num += DIGIT(a,i);
    DIGIT(a,i++) = num & (DIGIT_RADIX-1);
    num = num >> DIGIT_BIT;
  } while (i < a->used);

}

OBJECT bignum_new(STATE, int num) {
  mp_int *a;
  OBJECT o;
  o = object_memory_new_opaque(state, BASIC_CLASS(bignum), sizeof(mp_int));
  a = (mp_int*)BYTES_OF(o);
  mp_init(a);
  if(num < 0) {
    mp_set_int(a, (unsigned int)-num);
    a->sign = MP_NEG;
  } else {
    mp_set_int(a, (unsigned int)num);
  }
  return o;
}

OBJECT bignum_new_unsigned(STATE, unsigned int num) {
  mp_int *a;
  OBJECT o;
  o = object_memory_new_opaque(state, BASIC_CLASS(bignum), sizeof(mp_int));
  a = (mp_int*)BYTES_OF(o);
  mp_init(a);
  mp_set_int(a, num);
  return o;
}

static inline OBJECT bignum_normalize(STATE, OBJECT b) {
  if(mp_count_bits(MP(b)) <= FIXNUM_WIDTH) {
    int val;
    val = (int)mp_get_int(MP(b));
    if(MP(b)->sign == MP_NEG) {
      val = -val;
    }
    return I2N(val);
  }
  return b;
}

OBJECT bignum_add(STATE, OBJECT a, OBJECT b) {
  NMP;
  
  if(FIXNUM_P(b)) {
    b = bignum_new(state, FIXNUM_TO_INT(b));
  }
    
  mp_add(MP(a), MP(b), n);
  return bignum_normalize(state, n_obj);
}

OBJECT bignum_sub(STATE, OBJECT a, OBJECT b) {
  NMP;
  
  if(FIXNUM_P(b)) {
    b = bignum_new(state, FIXNUM_TO_INT(b));
  }
  
  mp_sub(MP(a), MP(b), n);
  return bignum_normalize(state, n_obj); 
}

OBJECT bignum_mul(STATE, OBJECT a, OBJECT b) {
  NMP;
  
  if(FIXNUM_P(b)) {
    if(b == I2N(2)) {
      mp_mul_2(MP(a), n);
      return n_obj;
    }
    b = bignum_new(state, FIXNUM_TO_INT(b));
  }
  
  mp_mul(MP(a), MP(b), n);
  return bignum_normalize(state, n_obj);
}

OBJECT bignum_div(STATE, OBJECT a, OBJECT b) {
  NMP;
  
  if(FIXNUM_P(b)) {
    b = bignum_new(state, FIXNUM_TO_INT(b));
  }
  
  mp_div(MP(a), MP(b), n, NULL);
  return bignum_normalize(state, n_obj);
}

OBJECT bignum_divmod(STATE, OBJECT a, OBJECT b) {
  NMP;
  MMP;
  OBJECT ary;
  
  mp_div(MP(a), MP(b), n, m);
  ary = array_new(state, 2);
  array_set(state, ary, 0, n_obj);
  array_set(state, ary, 1, m_obj);
  return ary;
}

int bignum_is_zero(STATE, OBJECT a) {
  return mp_iszero(MP(a)) ? TRUE : FALSE;
}

#define BITWISE_OP_AND 1
#define BITWISE_OP_OR  2
#define BITWISE_OP_XOR 3

void bignum_bitwise_op(int op, mp_int *x, mp_int *y, mp_int *n)
{
  mp_int   a,   b;
  mp_int *d1, *d2;
  int i, sign,  l1,  l2;
  mp_init(&a) ; mp_init(&b);

  if (y->sign == MP_NEG) {
    mp_copy(y, &b);
    twos_complement(&b);
    y = &b;
  }

  if (x->sign == MP_NEG) {
    mp_copy(x, &a);
    twos_complement(&a);
    x = &a;
  }

  if (x->used > y->used) {
    l1 = y->used;
    l2 = x->used;
    d1 = y;
    d2 = x;
    sign = y->sign;
  } else {
    l1 = x->used;
    l2 = y->used;
    d1 = x;
    d2 = y;
    sign = x->sign;
  }

  mp_grow(n, l2);
  n->used = l2;
  n->sign = MP_ZPOS;
  switch(op) {
    case BITWISE_OP_AND:
      if (x->sign == MP_NEG && y->sign == MP_NEG) n->sign = MP_NEG;
      for (i=0; i < l1; i++) {
        DIGIT(n,i) = DIGIT(d1,i) & DIGIT(d2,i);
      }
      for (; i < l2; i++) {
        DIGIT(n,i) = (sign == MP_ZPOS)?0:DIGIT(d2,i);
      }
      break;
    case BITWISE_OP_OR:
      if (x->sign == MP_NEG || y->sign == MP_NEG) n->sign = MP_NEG;
      for (i=0; i < l1; i++) {
        DIGIT(n,i) = DIGIT(d1,i) | DIGIT(d2,i);
      }
      for (; i < l2; i++) {
        DIGIT(n,i) = (sign == MP_ZPOS)?DIGIT(d2,i):(DIGIT_RADIX-1);
      }
      break;
    case BITWISE_OP_XOR:
      if (x->sign != y->sign) n->sign = MP_NEG;
      for (i=0; i < l1; i++) {
        DIGIT(n,i) = DIGIT(d1,i) ^ DIGIT(d2,i);
      }
      for (; i < l2; i++) {
        DIGIT(n,i) = (sign == MP_ZPOS)?DIGIT(d2,i):~DIGIT(d2,i);
      }
      break;
  }

  if (n->sign == MP_NEG) twos_complement(n);

  /* free allocated resources for twos complement copies */
  mp_clear(&a);
  mp_clear(&b);
}

OBJECT bignum_and(STATE, OBJECT a, OBJECT b) {
  NMP;

  if(FIXNUM_P(b)) {
    b = bignum_new(state, FIXNUM_TO_INT(b));
  }

  /* Perhaps this should use mp_and rather than our own version */
  bignum_bitwise_op(BITWISE_OP_AND, MP(a), MP(b), n);
  return bignum_normalize(state, n_obj);
}

OBJECT bignum_or(STATE, OBJECT a, OBJECT b) {
  NMP;

  if(FIXNUM_P(b)) {
    b = bignum_new(state, FIXNUM_TO_INT(b));
  }
  /* Perhaps this should use mp_or rather than our own version */
  bignum_bitwise_op(BITWISE_OP_OR, MP(a), MP(b), n);
  return bignum_normalize(state, n_obj);
}

OBJECT bignum_xor(STATE, OBJECT a, OBJECT b) {
  NMP;

  if(FIXNUM_P(b)) {
    b = bignum_new(state, FIXNUM_TO_INT(b));
  }
  /* Perhaps this should use mp_xor rather than our own version */
  bignum_bitwise_op(BITWISE_OP_XOR, MP(a), MP(b), n);
  return bignum_normalize(state, n_obj);
}

OBJECT bignum_invert(STATE, OBJECT self) {
  NMP;

  mp_int a; mp_init(&a);
  mp_int b; mp_init_set_int(&b, 1);

  /* inversion by -(a)-1 */
  mp_neg(MP(self), &a);
  mp_sub(&a, &b, n);

  mp_clear(&a); mp_clear(&b);
  return bignum_normalize(state, n_obj);
}

OBJECT bignum_neg(STATE, OBJECT self) {
  NMP;

  mp_neg(MP(self), n);
  return bignum_normalize(state, n_obj);
}

/* These 2 don't use mp_lshd because it shifts by internal digits,
   not bits. */

OBJECT bignum_left_shift(STATE, OBJECT self, OBJECT bits) {
  NMP;
  int shift = FIXNUM_TO_INT(bits);
  int s1 = shift / DIGIT_BIT;
  int s2 = shift % DIGIT_BIT;
  long len, i, j;

  mp_int * a;
  BDIGIT_DBL num = 0;

  a   = MP(self);
  len = a->used;

  n->sign = a->sign;
  mp_grow(n, len + s1 + 1);

  for (i=0; i < s1; i++) {
    DIGIT(n,i) = 0;
    n->used += 1;
  }

  for (j=0; j < len; j++) {
    num = num | (BDIGIT_DBL)DIGIT(a,j) << s2;
    DIGIT(n,i++) = (num & (DIGIT_RADIX-1));
    num = num >> DIGIT_BIT;
    n->used += 1;
  }

  DIGIT(n,i) = (num & (DIGIT_RADIX-1));
  n->used += 1;

  return bignum_normalize(state, n_obj);
}

OBJECT bignum_right_shift(STATE, OBJECT self, OBJECT bits) {
  NMP;
  int shift = FIXNUM_TO_INT(bits);
  long s1 = shift / DIGIT_BIT;
  long s2 = shift % DIGIT_BIT;

  BDIGIT_DBL num = 0;
  long i, j;

  mp_int * a;
  mp_int b;

  mp_init(&b);
  a =  MP(self);

  if (s1 > a->used) {
    if (a->sign == MP_ZPOS)
      return I2N(0);
    else
      return I2N(-1);
  }

  if (a->sign == MP_NEG) {
    mp_copy(a, &b);
    twos_complement(&b);
    a = &b;
  }

  i = a->used; 
  j = i - s1;

  if (j == 0) {
    if (a->sign == MP_ZPOS)
      return I2N(0);
    else
      return I2N(-1);
  }

  n->sign = a->sign;
  mp_grow(n, j);

  if (a->sign == MP_NEG) {
    num = ((BDIGIT_DBL)~0) << DIGIT_BIT;
  }
  while (i--, j--) {
    num = (num | DIGIT(a,i)) >> s2;
    DIGIT(n,j) = num & (DIGIT_RADIX-1);
    num = ((BDIGIT_DBL)DIGIT(a,i)) << DIGIT_BIT;
    n->used += 1;
  }

  if (a->sign == MP_NEG) {
    twos_complement(n);
  }

  mp_clear(&b);
  return bignum_normalize(state, n_obj);
}

OBJECT bignum_equal(STATE, OBJECT a, OBJECT b) {
  
  if(FIXNUM_P(b)) {
    b = bignum_new(state, FIXNUM_TO_INT(b));
  }
  
  if(mp_cmp(MP(a), MP(b)) == MP_EQ) {
    return Qtrue;
  }
  return Qfalse;
}

OBJECT bignum_compare(STATE, OBJECT a, OBJECT b) {
  switch(mp_cmp(MP(a), MP(b))) {
    case MP_LT:
      return I2N(-1);
    case MP_GT:
      return I2N(1);
  }
  return I2N(0);
}

unsigned long bignum_to_int(STATE, OBJECT self) {
  return mp_get_int(MP(self));
}

long long bignum_to_ll(STATE, OBJECT self) {
  mp_int t;
  mp_int *s = MP(self);
  long long out;
  
  out = (long long)mp_get_int(s);
  
  mp_init(&t);
  mp_copy(s, &t);
  mp_rshd(&t, sizeof(int));
  out = out | (mp_get_int(&t) << sizeof(int));
  mp_clear(&t);
  return out;
}

OBJECT bignum_from_ull(STATE, unsigned long long val) {
  OBJECT ret;
  mp_int a;
  mp_int b;
  mp_int *c;
  
  mp_init(&a);
  mp_set_int(&a, (unsigned int)(val & 0xffffffff));
  
  mp_lshd(&a, sizeof(int));
  mp_init(&b);
  mp_set_int(&b, (val >> sizeof(int)));
  
  ret = bignum_new_unsigned(state, 0);
  
  c = MP(ret);
  mp_or(&a, &b, c);
  
  return ret;
}

OBJECT bignum_from_ll(STATE, long long val) {
  OBJECT ret;
  
  ret = bignum_from_ull(state, (unsigned long long)val);
  
  if(val < 0) {
    MP(ret)->sign = MP_NEG;
  }
  
  return ret;
}

OBJECT bignum_to_s(STATE, OBJECT self, OBJECT radix) {
  char *buf;
  int sz;  
  int k;
  OBJECT obj;
  sz = 1024;
  for(;;) {
    buf = malloc(sizeof(char) * sz);
    mp_toradix_nd(MP(self), buf, FIXNUM_TO_INT(radix), sz, &k);
    if(k < sz - 2) {
      obj = string_new(state, buf);
      free(buf);
      return obj;
    }
    free(buf);
    sz += 1024;
  }
}

OBJECT bignum_from_string_detect(STATE, char *str) {
  char *s;
  int radix;
  int sign;
  NMP;
  s = str;
  sign = 1;
  while(isspace(*s)) { s++; }
  if(*s == '+') {
    s++;
  } else if(*s == '-') {
    sign = 0;
    s++;
  }
  radix = 10;
  if(*s == '0') {
    switch(s[1]) {
      case 'x': case 'X':
        radix = 16; s += 2;
        break;
      case 'b': case 'B':
        radix = 2; s += 2;
        break;
      case 'o': case 'O':
        radix = 8; s += 2;
        break;
      case 'd': case 'D':
        radix = 10; s += 2;
        break;
      default:
        radix = 8; s += 1;
    }
  }
  mp_read_radix(n, s, radix);
  
  if(!sign) { 
    n->sign = MP_NEG;
  }
  
  return bignum_normalize(state, n_obj);
}

OBJECT bignum_from_string(STATE, char *str, int radix) {
  NMP;
  mp_read_radix(n, str, radix);
  return bignum_normalize(state, n_obj);
}

void bignum_into_string(STATE, OBJECT self, int radix, char *buf, int sz) {
  int k;
  mp_toradix_nd(MP(self), buf, radix, sz, &k);
}

double bignum_to_double(STATE, OBJECT self) {
  int i;
  double res;
  double m;
  mp_int *a;

  a = MP(self);
  
  if (a->used == 0) {
     return 0;
  }

  /* get number of digits of the lsb we have to read */
  i = a->used;
  m = DIGIT_RADIX;

  /* get most significant digit of result */
  res = DIGIT(a,i);

  while (--i >= 0) {
    res = (res * m) + DIGIT(a,i);
  }

  if(isinf(res)) {
    /* Bignum out of range */
    res = HUGE_VAL;
  }

  if(a->sign == MP_NEG) res = -res;

  return res;
}

OBJECT bignum_from_double(STATE, double d)
{
  NMP;

  long i = 0;
  unsigned int c;
  double value;
  value = (d < 0) ? -d : d;

  /*
  if (isinf(d)) {
    rb_raise(rb_eFloatDomainError, d < 0 ? "-Infinity" : "Infinity");
  }
  if (isnan(d)) {
    rb_raise(rb_eFloatDomainError, "NaN");
  }
  */

  while (!(value <= (LONG_MAX >> 1)) || 0 != (long)value) {
    value = value / (double)(DIGIT_RADIX);
    i++;
  }

  mp_grow(n, i);
 
  while (i--) {
    value *= DIGIT_RADIX;
    c = (unsigned int)value;
    value -= c;
    DIGIT(n,i) = c;
    n->used += 1;
  }

  if (d < 0) {
    mp_neg(n, n);
  }

  return bignum_normalize(state, n_obj);
}

OBJECT bignum_size(STATE, OBJECT self)
{  
  int bits = mp_count_bits(MP(self));
  int bytes = bits / 8;
  
  if(bits % 8 > 0) {
    ++bytes;
  }
  
  /* MRI returns this in words, but thats an implementation detail as far
     as I'm concerned. */
  return I2N(bytes);
}
