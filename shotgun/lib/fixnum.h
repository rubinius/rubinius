#ifndef RBS_FIXNUM_H
#define RBS_FIXNUM_H

/* Each of these operations intend to implement safe code according to
   https://www.securecoding.cert.org/confluence/display/seccode/INT32-C.+Ensure+that+integer+operations+do+not+result+in+an+overflow
   Thanks defn52 for the link.
*/

static inline OBJECT fixnum_add(STATE, OBJECT a, OBJECT b) {
  OBJECT r;
  native_int j, k, m;
  
  // + will never overflow an int because of the size of a Fixnum
  j = FIXNUM_TO_INT(a);
  k = FIXNUM_TO_INT(b);
  m = j + k;
  r = I2N(m);
  if(m != FIXNUM_TO_INT(r)) {
    r = bignum_add(state, bignum_new(state, j), bignum_new(state, k));
  }
  
  return r;
}

static inline OBJECT fixnum_sub(STATE, OBJECT a, OBJECT b) {
  OBJECT r;
  native_int j, k, m;
  
  // - will never overflow a signed int because of the size of a Fixnum
  j = FIXNUM_TO_INT(a);
  k = FIXNUM_TO_INT(b);
  m = j - k;
  r = I2N(m);
  if(m != FIXNUM_TO_INT(r)) {
    r = bignum_sub(state, bignum_new(state, j), bignum_new(state, k));
  }
  
  return r;
}

static inline OBJECT fixnum_mul(STATE, OBJECT a, OBJECT b) {
  OBJECT r;
  long long m;
  native_int j, k;

  j = FIXNUM_TO_INT(a);
  k = FIXNUM_TO_INT(b);
  m = (long long)j * (long long)k;
  r = I2N((int) m);
  if(m != FIXNUM_TO_INT(r)) {
    r = bignum_mul(state, bignum_new(state, j), bignum_new(state, k));
  }
  
  return r;
}

static inline OBJECT fixnum_divmod(STATE, OBJECT a, OBJECT b) {
  OBJECT ary;
  long div, mod;
  native_int x, y;

  x = FIXNUM_TO_INT(a);
  y = FIXNUM_TO_INT(b);
  
  // adapted from ruby 1.8.x
  if (y < 0) {
    if (x < 0)
      div = -x / -y;
    else
      div = - (x / -y);
  } else {
    if (x < 0)
      div = - (-x / y);
    else
      div = x / y;
  }
  mod = x - div*y;
  if ((mod < 0 && y > 0) || (mod > 0 && y < 0)) {
    mod += y;
    div -= 1;
  }
  
  ary = array_new(state, 2);
  array_set(state, ary, 0, I2N(div));
  array_set(state, ary, 1, I2N(mod));
  return ary;
}

#endif
