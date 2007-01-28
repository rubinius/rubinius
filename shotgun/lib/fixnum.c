#include "shotgun.h"

/* Each of these operations intend to implement safe code according to
   https://www.securecoding.cert.org/confluence/display/seccode/INT32-C.+Ensure+that+integer+operations+do+not+result+in+an+overflow
   Thanks defn52 for the link.
*/

OBJECT fixnum_add(STATE, OBJECT a, OBJECT b) {
  OBJECT r;
  int j, k, m;
  
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

OBJECT fixnum_sub(STATE, OBJECT a, OBJECT b) {
  OBJECT r;
  int j, k, m;
  
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

OBJECT fixnum_mul(STATE, OBJECT a, OBJECT b) {
  OBJECT r;
  long long m;
  int j, k;

  j = FIXNUM_TO_INT(a);
  k = FIXNUM_TO_INT(b);
  m = (long long)j * (long long)k;
  r = I2N((int) m);
  if(m != FIXNUM_TO_INT(r)) {
    r = bignum_mul(state, bignum_new(state, j), bignum_new(state, k));
  }
  
  return r;
}

OBJECT fixnum_div(STATE, OBJECT a, OBJECT b) {
  OBJECT r;
  int j, k, m;
  
  j = FIXNUM_TO_INT(a);
  k = FIXNUM_TO_INT(b);
  m = j / k;
  r = I2N(m);
  if(m != FIXNUM_TO_INT(r)) {
    r = bignum_mul(state, bignum_new(state, j), bignum_new(state, k));
  }

  return r;
}
