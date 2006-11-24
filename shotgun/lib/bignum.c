#include "shotgun.h"
#include "tommath.h"
#include "string.h"

#define NMP mp_int *n; OBJECT n_obj; \
  NEW_STRUCT(n_obj, n, BASIC_CLASS(bignum), mp_int); \
  mp_init(n)

#define MP(k) DATA_STRUCT(k, mp_int*)

OBJECT bignum_new(STATE, int num) {
  mp_int *a;
  OBJECT o;
  o = object_memory_new_opaque(state, BASIC_CLASS(bignum), sizeof(mp_int));
  a = (mp_int*)BYTES_OF(o);  
  mp_init(a);
  mp_set_int(a, (unsigned int)num);
  if(num < 0) {
    a->sign = MP_NEG;
  }
  return o;
}

OBJECT bignum_normalize(STATE, OBJECT b) {
  if(mp_count_bits(MP(b)) <= (32 - 3)) {
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

OBJECT bignum_equal(STATE, OBJECT a, OBJECT b) {
  
  if(FIXNUM_P(b)) {
    b = bignum_new(state, FIXNUM_TO_INT(b));
  }
  
  if(!mp_cmp(MP(a), MP(b))) {
    return Qtrue;
  }
  return Qfalse;
}

OBJECT bignum_compare(STATE, OBJECT a, OBJECT b) {
  return I2N(mp_cmp(MP(a), MP(b)));
}

OBJECT bignum_to_s(STATE, OBJECT self, OBJECT radix) {
  char buffer[1024];
  int k;
  mp_toradix_nd(MP(self), buffer, FIXNUM_TO_INT(radix), 1024, &k);
  if(k >= 1022) {
    return Qnil;
  } else {
    return string_new(state, buffer);
  }
}

OBJECT bignum_from_string(STATE, char *str, int radix) {
  NMP;
  mp_read_radix(n, str, radix);
  return n_obj;
}

void bignum_into_string(STATE, OBJECT self, int radix, char *buf, int sz) {
  int k;
  mp_toradix_nd(MP(self), buf, radix, sz, &k);
}