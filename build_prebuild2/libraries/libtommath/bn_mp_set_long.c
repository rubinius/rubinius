#include <tommath.h>
#ifdef BN_MP_SET_LONG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 *
 * NOTE: This is a Rubinius extension to libtommath.
 */

/* set an unsigned long const */
int mp_set_long MPA(mp_int* a, unsigned long b) {
  mp_zero(a);

  while(b > MP_DIGIT_MAX) {
    a->dp[0] |= (b >> DIGIT_BIT);
    a->used += 1;
    int err = mp_mul_2d(MPST, a, DIGIT_BIT, a);
    if(err != MP_OKAY) {
      return err;
    }

    b &= MP_MASK;
  }

  a->dp[0] |= b;
  a->used += 1;

  mp_clamp(a);
  return MP_OKAY;
}
#endif
