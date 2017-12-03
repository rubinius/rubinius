#include <tommath.h>
#ifdef BN_MP_EXTEUCLID_C
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

/* Extended euclidean algorithm of (a, b) produces 
   a*u1 + b*u2 = u3
 */
int mp_exteuclid MPA(mp_int *a, mp_int *b, mp_int *U1, mp_int *U2, mp_int *U3)
{
   mp_int u1,u2,u3,v1,v2,v3,t1,t2,t3,q,tmp;
   int err;

   if ((err = mp_init_multi(&u1, &u2, &u3, &v1, &v2, &v3, &t1, &t2, &t3, &q, &tmp, NULL)) != MP_OKAY) {
      return err;
   }

   /* initialize, (u1,u2,u3) = (1,0,a) */
   mp_set(&u1, 1);
   if ((err = mp_copy(MPST, a, &u3)) != MP_OKAY)                                        { goto _ERR; }

   /* initialize, (v1,v2,v3) = (0,1,b) */
   mp_set(&v2, 1);
   if ((err = mp_copy(MPST, b, &v3)) != MP_OKAY)                                        { goto _ERR; }

   /* loop while v3 != 0 */
   while (mp_iszero(&v3) == MP_NO) {
       /* q = u3/v3 */
       if ((err = mp_div(MPST, &u3, &v3, &q, NULL)) != MP_OKAY)                         { goto _ERR; }

       /* (t1,t2,t3) = (u1,u2,u3) - (v1,v2,v3)q */
       if ((err = mp_mul(MPST, &v1, &q, &tmp)) != MP_OKAY)                              { goto _ERR; }
       if ((err = mp_sub(MPST, &u1, &tmp, &t1)) != MP_OKAY)                             { goto _ERR; }
       if ((err = mp_mul(MPST, &v2, &q, &tmp)) != MP_OKAY)                              { goto _ERR; }
       if ((err = mp_sub(MPST, &u2, &tmp, &t2)) != MP_OKAY)                             { goto _ERR; }
       if ((err = mp_mul(MPST, &v3, &q, &tmp)) != MP_OKAY)                              { goto _ERR; }
       if ((err = mp_sub(MPST, &u3, &tmp, &t3)) != MP_OKAY)                             { goto _ERR; }

       /* (u1,u2,u3) = (v1,v2,v3) */
       if ((err = mp_copy(MPST, &v1, &u1)) != MP_OKAY)                                  { goto _ERR; }
       if ((err = mp_copy(MPST, &v2, &u2)) != MP_OKAY)                                  { goto _ERR; }
       if ((err = mp_copy(MPST, &v3, &u3)) != MP_OKAY)                                  { goto _ERR; }

       /* (v1,v2,v3) = (t1,t2,t3) */
       if ((err = mp_copy(MPST, &t1, &v1)) != MP_OKAY)                                  { goto _ERR; }
       if ((err = mp_copy(MPST, &t2, &v2)) != MP_OKAY)                                  { goto _ERR; }
       if ((err = mp_copy(MPST, &t3, &v3)) != MP_OKAY)                                  { goto _ERR; }
   }

   /* make sure U3 >= 0 */
   if (u3.sign == MP_NEG) {
      mp_neg(MPST, &u1, &u1);
      mp_neg(MPST, &u2, &u2);
      mp_neg(MPST, &u3, &u3);
   }

   /* copy result out */
   if (U1 != NULL) { mp_managed_copy(MPST, &u1, U1); }
   if (U2 != NULL) { mp_managed_copy(MPST, &u2, U2); }
   if (U3 != NULL) { mp_managed_copy(MPST, &u3, U3); }

   err = MP_OKAY;
_ERR: mp_clear_multi(&u1, &u2, &u3, &v1, &v2, &v3, &t1, &t2, &t3, &q, &tmp, NULL);
   return err;
}
#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_exteuclid.c,v $ */
/* $Revision: 1.4 $ */
/* $Date: 2006/12/28 01:25:13 $ */
