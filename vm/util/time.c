#include <time.h>

/*
 * This was lifted wholesale from MRI, 1.8.7. I've externalized it a little
 * bit.
 *  - emp
 */

static int
leap_year_p(y)
    long y;
{
  return ((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0);
}

#define NDIV(x,y) (-(-((x)+1)/(y))-1)
#define DIV(n,d) ((n)<0 ? NDIV((n),(d)) : (n)/(d))

static time_t
timegm_noleapsecond(tm)
    struct tm *tm;
{
    static int common_year_yday_offset[] = {
        -1,
        -1 + 31,
        -1 + 31 + 28,
        -1 + 31 + 28 + 31,
        -1 + 31 + 28 + 31 + 30,
        -1 + 31 + 28 + 31 + 30 + 31,
        -1 + 31 + 28 + 31 + 30 + 31 + 30,
        -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31,
        -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
        -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
        -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
        -1 + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30
          /* 1    2    3    4    5    6    7    8    9    10   11 */
    };
    static int leap_year_yday_offset[] = {
        -1,
        -1 + 31,
        -1 + 31 + 29,
        -1 + 31 + 29 + 31,
        -1 + 31 + 29 + 31 + 30,
        -1 + 31 + 29 + 31 + 30 + 31,
        -1 + 31 + 29 + 31 + 30 + 31 + 30,
        -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31,
        -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,
        -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
        -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
        -1 + 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30
          /* 1    2    3    4    5    6    7    8    9    10   11 */
    };

    long tm_year = tm->tm_year;
    int tm_yday = tm->tm_mday;
    if (leap_year_p(tm_year + 1900))
        tm_yday += leap_year_yday_offset[tm->tm_mon];
    else
        tm_yday += common_year_yday_offset[tm->tm_mon];

    /*
     *  `Seconds Since the Epoch' in SUSv3:
     *  tm_sec + tm_min*60 + tm_hour*3600 + tm_yday*86400 +
     *  (tm_year-70)*31536000 + ((tm_year-69)/4)*86400 -
     *  ((tm_year-1)/100)*86400 + ((tm_year+299)/400)*86400
     */
    return tm->tm_sec + tm->tm_min*60 + tm->tm_hour*3600 +
           (time_t)(tm_yday +
                    (tm_year-70)*365 +
                    DIV(tm_year-69,4) -
                    DIV(tm_year-1,100) +
                    DIV(tm_year+299,400))*86400;
}

static int
tmcmp(a, b)
    struct tm *a;
    struct tm *b;
{
    if (a->tm_year != b->tm_year)
        return a->tm_year < b->tm_year ? -1 : 1;
    else if (a->tm_mon != b->tm_mon)
        return a->tm_mon < b->tm_mon ? -1 : 1;
    else if (a->tm_mday != b->tm_mday)
        return a->tm_mday < b->tm_mday ? -1 : 1;
    else if (a->tm_hour != b->tm_hour)
        return a->tm_hour < b->tm_hour ? -1 : 1;
    else if (a->tm_min != b->tm_min)
        return a->tm_min < b->tm_min ? -1 : 1;
    else if (a->tm_sec != b->tm_sec)
        return a->tm_sec < b->tm_sec ? -1 : 1;
    else
        return 0;
}

// This code only works if sizeof(time_t) == sizeof(long)
typedef unsigned long unsigned_time_t;

// lifted from MRI's configure on OS X, probably right on most unix platforms?
#define NEGATIVE_TIME_T 1

time_t
mktime_extended(struct tm* tptr, int utc_p, int* err) {
    time_t guess, guess_lo, guess_hi;
    struct tm *tm, tm_lo, tm_hi;
    int d, have_guess;
    int find_dst;

    // Start off using there is no error.
    *err = 0;

    find_dst = 0 < tptr->tm_isdst;

#ifdef NEGATIVE_TIME_T
    guess_lo = (time_t)~((unsigned_time_t)~(time_t)0 >> 1);
#else
    guess_lo = 0;
#endif
    guess_hi = ((time_t)-1) < ((time_t)0) ?
	       (time_t)((unsigned_time_t)~(time_t)0 >> 1) :
	       ~(time_t)0;

    guess = timegm_noleapsecond(tptr);
    tm = (utc_p ? gmtime : localtime)(&guess);
    if (tm) {
        d = tmcmp(tptr, tm);
        if (d == 0) return guess;
        if (d < 0) {
            guess_hi = guess;
            guess -= 24 * 60 * 60;
        }
        else {
            guess_lo = guess;
            guess += 24 * 60 * 60;
        }
        if (guess_lo < guess && guess < guess_hi &&
            (tm = (utc_p ? gmtime : localtime)(&guess)) != NULL) {
            d = tmcmp(tptr, tm);
            if (d == 0) return guess;
            if (d < 0)
                guess_hi = guess;
            else
                guess_lo = guess;
        }
    }

    tm = (utc_p ? gmtime : localtime)(&guess_lo);
    if (!tm) goto error;
    d = tmcmp(tptr, tm);
    if (d < 0) goto out_of_range;
    if (d == 0) return guess_lo;
    tm_lo = *tm;

    tm = (utc_p ? gmtime : localtime)(&guess_hi);
    if (!tm) goto error;
    d = tmcmp(tptr, tm);
    if (d > 0) goto out_of_range;
    if (d == 0) return guess_hi;
    tm_hi = *tm;

    have_guess = 0;

    while (guess_lo + 1 < guess_hi) {
      /* there is a gap between guess_lo and guess_hi. */
      unsigned long range = 0;
      if (!have_guess) {
	int a, b;
	/*
	  Try precious guess by a linear interpolation at first.
	  `a' and `b' is a coefficient of guess_lo and guess_hi as:

	    guess = (guess_lo * a + guess_hi * b) / (a + b)

	  However this causes overflow in most cases, following assignment
	  is used instead:

	    guess = guess_lo / d * a + (guess_lo % d) * a / d
		  + guess_hi / d * b + (guess_hi % d) * b / d
	      where d = a + b

	  To avoid overflow in this assignment, `d' is restricted to less than
	  sqrt(2**31).  By this restriction and other reasons, the guess is
	  not accurate and some error is expected.  `range' approximates 
	  the maximum error.

	  When these parameters are not suitable, i.e. guess is not within
	  guess_lo and guess_hi, simple guess by binary search is used.
	*/
	range = 366 * 24 * 60 * 60;
	a = (tm_hi.tm_year - tptr->tm_year);
	b = (tptr->tm_year - tm_lo.tm_year);
	/* 46000 is selected as `some big number less than sqrt(2**31)'. */
	if (a + b <= 46000 / 12) {
	  range = 31 * 24 * 60 * 60;
	  a *= 12;
	  b *= 12;
	  a += tm_hi.tm_mon - tptr->tm_mon;
	  b += tptr->tm_mon - tm_lo.tm_mon;
	  if (a + b <= 46000 / 31) {
	    range = 24 * 60 * 60;
	    a *= 31;
	    b *= 31;
	    a += tm_hi.tm_mday - tptr->tm_mday;
	    b += tptr->tm_mday - tm_lo.tm_mday;
	    if (a + b <= 46000 / 24) {
	      range = 60 * 60;
	      a *= 24;
	      b *= 24;
	      a += tm_hi.tm_hour - tptr->tm_hour;
	      b += tptr->tm_hour - tm_lo.tm_hour;
	      if (a + b <= 46000 / 60) {
		range = 60;
		a *= 60;
		b *= 60;
		a += tm_hi.tm_min - tptr->tm_min;
		b += tptr->tm_min - tm_lo.tm_min;
		if (a + b <= 46000 / 60) {
		  range = 1;
		  a *= 60;
		  b *= 60;
		  a += tm_hi.tm_sec - tptr->tm_sec;
		  b += tptr->tm_sec - tm_lo.tm_sec;
		}
	      }
	    }
	  }
	}
	if (a <= 0) a = 1;
	if (b <= 0) b = 1;
	d = a + b;
	/*
	  Although `/' and `%' may produce unexpected result with negative
	  argument, it doesn't cause serious problem because there is a
	  fail safe.
	*/
	guess = guess_lo / d * a + (guess_lo % d) * a / d
	      + guess_hi / d * b + (guess_hi % d) * b / d;
	have_guess = 1;
      }

      if (guess <= guess_lo || guess_hi <= guess) {
	/* Precious guess is invalid. try binary search. */ 
	guess = guess_lo / 2 + guess_hi / 2;
	if (guess <= guess_lo)
	  guess = guess_lo + 1;
	else if (guess >= guess_hi)
	  guess = guess_hi - 1;
	range = 0;
      }

      tm = (utc_p ? gmtime : localtime)(&guess);
      if (!tm) goto error;
      have_guess = 0;

      d = tmcmp(tptr, tm);
      if (d < 0) {
        guess_hi = guess;
	tm_hi = *tm;
	if (range) {
	  guess = guess - range;
	  range = 0;
	  if (guess_lo < guess && guess < guess_hi)
	    have_guess = 1;
	}
      }
      else if (d > 0) {
        guess_lo = guess;
	tm_lo = *tm;
	if (range) {
	  guess = guess + range;
	  range = 0;
	  if (guess_lo < guess && guess < guess_hi)
	    have_guess = 1;
	}
      }
      else {
	if (!utc_p) {
	  /* If localtime is nonmonotonic, another result may exist. */
	  time_t guess2;
	  if (find_dst) {
	    guess2 = guess - 2 * 60 * 60;
	    tm = localtime(&guess2);
	    if (tm) {
	      if (tptr->tm_hour != (tm->tm_hour + 2) % 24 ||
		  tptr->tm_min != tm->tm_min ||
		  tptr->tm_sec != tm->tm_sec) {
		guess2 -= (tm->tm_hour - tptr->tm_hour) * 60 * 60 +
			  (tm->tm_min - tptr->tm_min) * 60 +
			  (tm->tm_sec - tptr->tm_sec);
		if (tptr->tm_mday != tm->tm_mday)
		  guess2 += 24 * 60 * 60;
		if (guess != guess2) {
		  tm = localtime(&guess2);
		  if (tmcmp(tptr, tm) == 0) {
		    if (guess < guess2)
		      return guess;
		    else
		      return guess2;
		  }
		}
	      }
	    }
	  }
	  else {
	    guess2 = guess + 2 * 60 * 60;
	    tm = localtime(&guess2);
	    if (tm) {
	      if ((tptr->tm_hour + 2) % 24 != tm->tm_hour ||
		  tptr->tm_min != tm->tm_min ||
		  tptr->tm_sec != tm->tm_sec) {
		guess2 -= (tm->tm_hour - tptr->tm_hour) * 60 * 60 +
			  (tm->tm_min - tptr->tm_min) * 60 +
			  (tm->tm_sec - tptr->tm_sec);
		if (tptr->tm_mday != tm->tm_mday)
		  guess2 -= 24 * 60 * 60;
		if (guess != guess2) {
		  tm = localtime(&guess2);
		  if (tmcmp(tptr, tm) == 0) {
		    if (guess < guess2)
		      return guess2;
		    else
		      return guess;
		  }
		}
	      }
	    }
	  }
	}
	return guess;
      }
    }
    /* Given argument has no corresponding time_t. Let's outerpolation. */
    if (tm_lo.tm_year == tptr->tm_year && tm_lo.tm_mon == tptr->tm_mon) {
      return guess_lo +
        (tptr->tm_mday - tm_lo.tm_mday) * 24 * 60 * 60 +
        (tptr->tm_hour - tm_lo.tm_hour) * 60 * 60 +
        (tptr->tm_min - tm_lo.tm_min) * 60 +
        (tptr->tm_sec - tm_lo.tm_sec);
    }
    else if (tm_hi.tm_year == tptr->tm_year && tm_hi.tm_mon == tptr->tm_mon) {
      return guess_hi +
        (tptr->tm_mday - tm_hi.tm_mday) * 24 * 60 * 60 +
        (tptr->tm_hour - tm_hi.tm_hour) * 60 * 60 +
        (tptr->tm_min - tm_hi.tm_min) * 60 +
        (tptr->tm_sec - tm_hi.tm_sec);
    }

error:
out_of_range:
    *err = 1;
    return 0;
}
