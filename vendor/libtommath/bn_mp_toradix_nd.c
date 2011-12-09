#include <tommath.h>
#ifdef BN_MP_TORADIX_N_C
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
 * Tom St Denis, tomstdenis@gmail.com, http://math.libtomcrypt.com
 */

/*
 * Maps for how big the bases are we can do modulo and division
 * for when determining the string representation
 */

#ifdef MP_64BIT
static const mp_digit bases[35][2] = {
  { 63, 9223372036854775808UL },
  { 39, 4052555153018976267UL },
  { 31, 4611686018427387904UL },
  { 26, 1490116119384765625UL },
  { 23, 789730223053602816UL },
  { 22, 3909821048582988049UL },
  { 20, 1152921504606846976UL },
  { 19, 1350851717672992089UL },
  { 18, 1000000000000000000UL },
  { 17, 505447028499293771UL },
  { 17, 2218611106740436992UL },
  { 16, 665416609183179841UL },
  { 16, 2177953337809371136UL },
  { 15, 437893890380859375UL },
  { 15, 1152921504606846976UL },
  { 15, 2862423051509815793UL },
  { 14, 374813367582081024UL },
  { 14, 799006685782884121UL },
  { 14, 1638400000000000000UL },
  { 14, 3243919932521508681UL },
  { 13, 282810057883082752UL },
  { 13, 504036361936467383UL },
  { 13, 876488338465357824UL },
  { 13, 1490116119384765625UL },
  { 13, 2481152873203736576UL },
  { 13, 4052555153018976267UL },
  { 12, 232218265089212416UL },
  { 12, 353814783205469041UL },
  { 12, 531441000000000000UL },
  { 12, 787662783788549761UL },
  { 12, 1152921504606846976UL },
  { 12, 1667889514952984961UL },
  { 12, 2386420683693101056UL },
  { 12, 3379220508056640625UL },
  { 11, 131621703842267136UL }
};
#else
static const mp_digit bases[35][2] = {
  { 31, 2147483648UL },
  { 19, 1162261467UL },
  { 15, 1073741824UL },
  { 13, 1220703125UL },
  { 11, 362797056UL },
  { 11, 1977326743UL },
  { 10, 1073741824UL },
  { 9, 387420489UL },
  { 9, 1000000000UL },
  { 8, 214358881UL },
  { 8, 429981696UL },
  { 8, 815730721UL },
  { 8, 1475789056UL },
  { 7, 170859375UL },
  { 7, 268435456UL },
  { 7, 410338673UL },
  { 7, 612220032UL },
  { 7, 893871739UL },
  { 7, 1280000000UL },
  { 7, 1801088541UL },
  { 6, 113379904UL },
  { 6, 148035889UL },
  { 6, 191102976UL },
  { 6, 244140625UL },
  { 6, 308915776UL },
  { 6, 387420489UL },
  { 6, 481890304UL },
  { 6, 594823321UL },
  { 6, 729000000UL },
  { 6, 887503681UL },
  { 6, 1073741824UL },
  { 6, 1291467969UL },
  { 6, 1544804416UL },
  { 6, 1838265625UL },
  { 5, 60466176UL },
};
#endif

/* stores a bignum as a ASCII string in a given radix (2..64) 
 *
 * Stores upto maxlen-1 chars and always a NULL byte
 * Indicate how many digits were used in digits
 */
int mp_toradix_nd MPA(mp_int * a, char *str, int radix, int maxlen, int *digits)
{
  int     res;
  mp_int  t;

  /* check range of the maxlen, radix */
  if (maxlen < 2 || radix < 2 || radix > 64) {
    return MP_VAL;
  }

  /* quick out if its zero */
  if (mp_iszero(a) == MP_YES) {
     *str++ = '0';
     *str = '\0';
     *digits = 1;
     return MP_OKAY;
  }

  if ((res = mp_init_copy (MPST, &t, a)) != MP_OKAY) {
    return res;
  }



  int i = t.used, j = maxlen, digs = 0;
  mp_digit hbase = bases[radix - 2][1];

  str[--j] = '\0';
  while (i && j > 0) {
    int k = i;
    mp_word num = 0;

    while (k--) {               /* x / hbase */
      num = (num << DIGIT_BIT) + t.dp[k];
      t.dp[k] = (mp_digit)(num / hbase);
      num %= hbase;
    }
    if (t.dp[i-1] == 0) i--;
    k = bases[radix - 2][0];

    mp_digit num_base = (mp_digit) num;
    while (k--) {
      str[--j] = mp_s_rmap[num_base % radix];
      digs++;
      num_base /= radix;
      if (j <= 0) break;
      if (i == 0 && num_base == 0) break;
    }
  }

  if (t.sign == MP_NEG) {
    /* store the flag and mark the number as positive */
    str[--j] = '-';
  }

  if(j > 0) {
    memmove(str, str + j, maxlen - j);
  }

  *digits = digs;

  mp_clear(&t);

  return MP_OKAY;
}

#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_toradix_n.c,v $ */
/* $Revision: 1.4 $ */
/* $Date: 2006/03/31 14:18:44 $ */
