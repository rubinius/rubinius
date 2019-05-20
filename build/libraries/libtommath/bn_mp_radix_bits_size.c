#include <tommath.h>
#include <math.h>
#ifdef BN_MP_RADIX_BITS_SIZE_C
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

/* Returns the maximum size of ASCII representation.
   This is a fast way to determine the maximum needed size
   for representing this mp_int in a string with the given
   radix. This allows for preallocating a buffer for
   mp_toradix_* functions without having to do the much
   more expensive mp_radix_size() computations.
*/
int mp_radix_bits_size(int bits, int radix, int *size)
{
  /* We use a log table since we only need the log2 values
     for 2 to 64. This allows for now having to compute this
     at runtime */
  static const double logs[] = {
    1.0,
    1.584962500721156,
    2.0,
    2.321928094887362,
    2.584962500721156,
    2.807354922057604,
    3.0,
    3.169925001442312,
    3.321928094887362,
    3.4594316186372973,
    3.584962500721156,
    3.700439718141092,
    3.807354922057604,
    3.9068905956085187,
    4.0,
    4.087462841250339,
    4.169925001442312,
    4.247927513443585,
    4.321928094887363,
    4.392317422778761,
    4.459431618637297,
    4.523561956057013,
    4.584962500721156,
    4.643856189774724,
    4.700439718141092,
    4.754887502163468,
    4.807354922057604,
    4.857980995127572,
    4.906890595608519,
    4.954196310386875,
    5.0,
    5.044394119358453,
    5.087462841250339,
    5.129283016944966,
    5.169925001442312,
    5.20945336562895,
    5.247927513443585,
    5.285402218862249,
    5.321928094887363,
    5.357552004618084,
    5.392317422778761,
    5.426264754702098,
    5.459431618637297,
    5.491853096329675,
    5.523561956057013,
    5.554588851677638,
    5.584962500721156,
    5.614709844115208,
    5.643856189774724,
    5.672425341971495,
    5.700439718141092,
    5.727920454563199,
    5.754887502163468,
    5.78135971352466,
    5.807354922057604,
    5.832890014164741,
    5.857980995127572,
    5.882643049361842,
    5.906890595608519,
    5.930737337562887,
    5.954196310386875,
    5.977279923499917,
    6.0
  };

  /* make sure the radix is in range */
  if (radix < 2 || radix > 64) {
    return MP_VAL;
  }

  *size = (int) ceil(bits / logs[radix - 2]);
  return MP_OKAY;
}

#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_radix_size.c,v $ */
/* $Revision: 1.5 $ */
/* $Date: 2006/12/28 01:25:13 $ */
