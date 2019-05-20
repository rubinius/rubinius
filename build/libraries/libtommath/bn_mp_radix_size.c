#include <tommath.h>
#ifdef BN_MP_RADIX_SIZE_C
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
 */

/* returns size of ASCII reprensentation */
int mp_radix_size(mp_int * a, int radix, int *size)
{
  int bits;

  /* make sure the radix is in range */
  if (radix < 2 || radix > 64) {
    return MP_VAL;
  }

  if (mp_iszero(a) == MP_YES) {
    *size = 2;
    return MP_OKAY;
  }

  bits = mp_count_bits(a);
  mp_radix_bits_size(bits, radix, size);
  *size += a->sign == MP_NEG ? 2 : 1;
  return MP_OKAY;
}

#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_radix_size.c,v $ */
/* $Revision: 1.5 $ */
/* $Date: 2006/12/28 01:25:13 $ */
