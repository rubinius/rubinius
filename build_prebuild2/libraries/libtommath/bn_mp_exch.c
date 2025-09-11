#include <tommath.h>
#ifdef BN_MP_EXCH_C
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

/* swap the elements of two integers, for cases where you can't simply swap the 
 * mp_int pointers around
 */
void
mp_exch MPA(mp_int * a, mp_int * b)
{
  mp_int  t;

  if(MANAGED(a) || MANAGED(b)) {
    mp_init(&t);

    // copy a to t
    mp_copy(MPST, a, &t);

    // copy b to a
    mp_copy(MPST, b, a);

    // copy t to b
    mp_copy(MPST, &t, b);

    mp_clear(&t);
    return;
  }

  t  = *a;
  *a = *b;
  *b = t;
}
#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_exch.c,v $ */
/* $Revision: 1.4 $ */
/* $Date: 2006/12/28 01:25:13 $ */
