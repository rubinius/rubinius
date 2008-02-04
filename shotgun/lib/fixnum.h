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
  j = N2I(a);
  k = N2I(b);
  m = j + k;
  r = I2N(m);
  if(m != N2I(r)) {
    r = bignum_add(state, bignum_new(state, j), bignum_new(state, k));
  }
  
  return r;
}

static inline OBJECT fixnum_sub(STATE, OBJECT a, OBJECT b) {
  OBJECT r;
  native_int j, k, m;
  
  // - will never overflow a signed int because of the size of a Fixnum
  j = N2I(a);
  k = N2I(b);
  m = j - k;
  r = I2N(m);
  if(m != N2I(r)) {
    r = bignum_sub(state, bignum_new(state, j), bignum_new(state, k));
  }
  
  return r;
}

static inline OBJECT fixnum_mul(STATE, OBJECT a, OBJECT b) {
  native_int na = N2I(a);
  native_int nb = N2I(b);
  unsigned int overflow = FALSE;

  /* There is no C type large enough to (always) hold the result of
   * multiplying two native_ints together
   */

  if (na > 0) { /* a is positive */
    if (nb > 0) { /* a and b are positive */
      if (na > (FIXNUM_MAX / nb)) { overflow = TRUE; }
    } else { /* a is positive, b is non-positive */
      if (nb < (FIXNUM_MIN / na)) { overflow = TRUE; }
    }
  } else { /* a is non-positive */
    if (nb > 0) { /* a is non-positive, b is positive */
      if (na < (FIXNUM_MIN / nb)) { overflow = TRUE; }
    } else { /* a and b are non-positive */
      if ( (na != 0) && (nb < (FIXNUM_MAX / na))) { overflow = TRUE; }
    }
  }

  if (overflow) {
    return bignum_mul(state, bignum_new(state, na), bignum_new(state, nb));
  } else {
    return I2N(na * nb);
  }
}

static inline OBJECT fixnum_divmod(STATE, OBJECT a, OBJECT b) {
  OBJECT ary;
  long div, mod;
  native_int x, y;

  x = N2I(a);
  y = N2I(b);
  
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
