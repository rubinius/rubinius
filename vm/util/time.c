#include <time.h>

/* Prevent NetBSD from using ctype macros that cause compiler
 * warnings about char as subscript.
 */
#define _CTYPE_NOINLINE 1

/*
 * This was lifted wholesale from MRI, 1.8.7. I've externalized it a little
 * bit.
 *  - emp
 */

static int
leap_year_p(long y)
{
  return ((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0);
}

#define NDIV(x,y) (-(-((x)+1)/(y))-1)
#define DIV(n,d) ((n)<0 ? NDIV((n),(d)) : (n)/(d))

static time_t
timegm_noleapsecond(struct tm* tm)
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
tmcmp(struct tm* a, struct tm* b)
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

/* -*- c-file-style: "linux" -*- */

/*
 * strftime.c
 *
 * Public-domain implementation of ANSI C library routine.
 *
 * It's written in old-style C for maximal portability.
 * However, since I'm used to prototypes, I've included them too.
 *
 * If you want stuff in the System V ascftime routine, add the SYSV_EXT define.
 * For extensions from SunOS, add SUNOS_EXT.
 * For stuff needed to implement the P1003.2 date command, add POSIX2_DATE.
 * For VMS dates, add VMS_EXT.
 * For a an RFC822 time format, add MAILHEADER_EXT.
 * For ISO week years, add ISO_DATE_EXT.
 * For complete POSIX semantics, add POSIX_SEMANTICS.
 *
 * The code for %c, %x, and %X now follows the 1003.2 specification for
 * the POSIX locale.
 * This version ignores LOCALE information.
 * It also doesn't worry about multi-byte characters.
 * So there.
 *
 * This file is also shipped with GAWK (GNU Awk), gawk specific bits of
 * code are included if GAWK is defined.
 *
 * Arnold Robbins
 * January, February, March, 1991
 * Updated March, April 1992
 * Updated April, 1993
 * Updated February, 1994
 * Updated May, 1994
 * Updated January, 1995
 * Updated September, 1995
 * Updated January, 1996
 *
 * Fixes from ado@elsie.nci.nih.gov
 * February 1991, May 1992
 * Fixes from Tor Lillqvist tml@tik.vtt.fi
 * May, 1993
 * Further fixes from ado@elsie.nci.nih.gov
 * February 1994
 * %z code from chip@chinacat.unicom.com
 * Applied September 1995
 * %V code fixed (again) and %G, %g added,
 * January 1996
 */

// Modernized and cleaned up by Evan Phoenix - 2010

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>

#include <math.h>

#include "vm/config.h"

#ifndef RBX_WINDOWS
#define HAVE_TM_ZONE    1
#endif

/* defaults: season to taste */
#define SYSV_EXT	1	/* stuff in System V ascftime routine */
#define SUNOS_EXT	1	/* stuff in SunOS strftime routine */
#define POSIX2_DATE	1	/* stuff in Posix 1003.2 date command */
#define VMS_EXT		1	/* include %v for VMS date format */
#define MAILHEADER_EXT	1	/* add %z for HHMM format */
#define ISO_DATE_EXT	1	/* %G and %g for year of ISO week */
#ifndef GAWK
#define POSIX_SEMANTICS	1	/* call tzset() if TZ changes */
#endif

#if defined(ISO_DATE_EXT)
#if ! defined(POSIX2_DATE)
#define POSIX2_DATE	1
#endif
#endif

#if defined(POSIX2_DATE)
#if ! defined(SYSV_EXT)
#define SYSV_EXT	1
#endif
#if ! defined(SUNOS_EXT)
#define SUNOS_EXT	1
#endif
#endif

#if defined(POSIX2_DATE)
#define adddecl(stuff)	stuff
#else
#define adddecl(stuff)
#endif

#undef strchr	/* avoid AIX weirdness */

static int weeknumber(const struct tm *timeptr, int firstweekday);
adddecl(static int iso8601wknum(const struct tm *timeptr);)

#include <stdlib.h>
#include <string.h>

#if defined __WIN32__ || defined _WIN32
#define DLL_IMPORT __declspec(dllimport)
#endif
#ifndef DLL_IMPORT
#define DLL_IMPORT
#endif
#if !defined(OS2) && defined(HAVE_TZNAME)
extern DLL_IMPORT char *tzname[2];
#ifdef HAVE_DAYLIGHT
extern DLL_IMPORT int daylight;
#endif
#ifdef HAVE_VAR_TIMEZONE
extern DLL_IMPORT TYPEOF_VAR_TIMEZONE timezone;
#endif
#ifdef HAVE_VAR_ALTZONE
extern DLL_IMPORT TYPEOF_VAR_ALTZONE altzone;
#endif
#endif

#undef min	/* just in case */

/* min --- return minimum of two numbers */

static inline int min(int a, int b) {
	return (a < b ? a : b);
}

#undef max	/* also, just in case */

/* max --- return maximum of two numbers */

static inline int max(int a, int b) {
	return (a > b ? a : b);
}

#define range(low, item, hi)	max(low, min(item, hi))

/* strftime --- produce formatted time */

size_t
strftime_extended(char *s, size_t maxsize, const char *format, const struct tm *timeptr, const struct timespec *ts, int gmt)
{
	char *endp = s + maxsize;
	char *start = s;
	const char *sp, *tp;
	auto char tbuf[100];
	long off;
	int i, w;
	long y;
	static short first = 1;
#ifdef POSIX_SEMANTICS
	static char *savetz = NULL;
	static int savetzlen = 0;
	char *tz;
#endif /* POSIX_SEMANTICS */
#ifndef HAVE_TM_ZONE
#ifndef HAVE_TM_NAME
#if ((defined(MAILHEADER_EXT) && !HAVE_VAR_TIMEZONE && HAVE_GETTIMEOFDAY) || \
     (!HAVE_TZNAME && HAVE_TIMEZONE))
	struct timeval tv;
	struct timezone zone;
#endif
#endif /* HAVE_TM_NAME */
#endif /* HAVE_TM_ZONE */
	int precision, flags;
	char padding;
	enum {LEFT, CHCASE, LOWER, UPPER, LOCALE_O, LOCALE_E};
#define BIT_OF(n) (1U<<(n))

	/* various tables, useful in North America */
	static const char days_l[][10] = {
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday",
	};
	static const char months_l[][10] = {
		"January", "February", "March", "April",
		"May", "June", "July", "August", "September",
		"October", "November", "December",
	};
	static const char ampm[][3] = { "AM", "PM", };

	if (s == NULL || format == NULL || timeptr == NULL || maxsize == 0)
		return 0;

	/* quick check if we even need to bother */
	if (strchr(format, '%') == NULL && strlen(format) + 1 >= maxsize) {
	err:
		errno = ERANGE;
		return 0;
	}

#ifndef POSIX_SEMANTICS
	if (first) {
		tzset();
		first = 0;
	}
#else	/* POSIX_SEMANTICS */
	tz = getenv("TZ");
	if (first) {
		if (tz != NULL) {
			int tzlen = strlen(tz);

			savetz = (char *) malloc(tzlen + 1);
			if (savetz != NULL) {
				savetzlen = tzlen + 1;
				strcpy(savetz, tz);
			}
		}
		tzset();
		first = 0;
	}
	/* if we have a saved TZ, and it is different, recapture and reset */
	if (tz && savetz && (tz[0] != savetz[0] || strcmp(tz, savetz) != 0)) {
		i = strlen(tz) + 1;
		if (i > savetzlen) {
			savetz = (char *) realloc(savetz, i);
			if (savetz) {
				savetzlen = i;
				strcpy(savetz, tz);
			}
		} else
			strcpy(savetz, tz);
		tzset();
	}
#endif	/* POSIX_SEMANTICS */

	for (; *format && s < endp - 1; format++) {
#define FLAG_FOUND() do { \
			if (precision > 0 || flags & (BIT_OF(LOCALE_E)|BIT_OF(LOCALE_O))) \
				goto unknown; \
		} while (0)
#define NEEDS(n) do if (s + (n) >= endp - 1) goto err; while (0)
#define FILL_PADDING(i) do { \
	if (!(flags & BIT_OF(LEFT)) && precision > i) { \
		NEEDS(precision); \
		memset(s, padding ? padding : ' ', precision - i); \
		s += precision - i; \
	} \
	else { \
		NEEDS(i); \
	} \
} while (0);
#define FMT(def_pad, def_prec, fmt, val) \
		do { \
			int l; \
			if (precision <= 0) precision = (def_prec); \
			if (flags & BIT_OF(LEFT)) precision = 1; \
			l = snprintf(s, endp - s, \
				     ((padding == '0' || (!padding && def_pad == '0')) ? "%0*"fmt : "%*"fmt), \
				     precision, val); \
			if (l < 0) goto err; \
			s += l; \
		} while (0)
#define STRFTIME(fmt) \
		do { \
			i = strftime_extended(s, endp - s, fmt, timeptr, ts, gmt); \
			if (!i) return 0; \
			if (precision > i) {\
				memmove(s + precision - i, s, i);\
				memset(s, padding ? padding : ' ', precision - i); \
				s += precision;	\
	                }\
			else s += i; \
		} while (0)

		if (*format != '%') {
			*s++ = *format;
			continue;
		}
		tp = tbuf;
		sp = format;
		precision = -1;
		flags = 0;
		padding = 0;
	again:
		switch (*++format) {
		case '\0':
			format--;
			goto unknown;

		case '%':
			FILL_PADDING(1);
			*s++ = '%';
			continue;

		case 'a':	/* abbreviated weekday name */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(LOWER)|BIT_OF(CHCASE));
				flags |= BIT_OF(UPPER);
			}
			if (timeptr->tm_wday < 0 || timeptr->tm_wday > 6)
				i = 1, tp = "?";
			else
				i = 3, tp = days_l[timeptr->tm_wday];
			break;

		case 'A':	/* full weekday name */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(LOWER)|BIT_OF(CHCASE));
				flags |= BIT_OF(UPPER);
			}
			if (timeptr->tm_wday < 0 || timeptr->tm_wday > 6)
				i = 1, tp = "?";
			else
				i = strlen(tp = days_l[timeptr->tm_wday]);
			break;

#ifdef SYSV_EXT
		case 'h':	/* abbreviated month name */
#endif
		case 'b':	/* abbreviated month name */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(LOWER)|BIT_OF(CHCASE));
				flags |= BIT_OF(UPPER);
			}
			if (timeptr->tm_mon < 0 || timeptr->tm_mon > 11)
				i = 1, tp = "?";
			else
				i = 3, tp = months_l[timeptr->tm_mon];
			break;

		case 'B':	/* full month name */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(LOWER)|BIT_OF(CHCASE));
				flags |= BIT_OF(UPPER);
			}
			if (timeptr->tm_mon < 0 || timeptr->tm_mon > 11)
				i = 1, tp = "?";
			else
				i = strlen(tp = months_l[timeptr->tm_mon]);
			break;

		case 'c':	/* appropriate date and time representation */
			STRFTIME("%a %b %e %H:%M:%S %Y");
			continue;

		case 'd':	/* day of the month, 01 - 31 */
			i = range(1, timeptr->tm_mday, 31);
			FMT('0', 2, "d", i);
			continue;

		case 'H':	/* hour, 24-hour clock, 00 - 23 */
			i = range(0, timeptr->tm_hour, 23);
			FMT('0', 2, "d", i);
			continue;

		case 'I':	/* hour, 12-hour clock, 01 - 12 */
			i = range(0, timeptr->tm_hour, 23);
			if (i == 0)
				i = 12;
			else if (i > 12)
				i -= 12;
			FMT('0', 2, "d", i);
			continue;

		case 'j':	/* day of the year, 001 - 366 */
			FMT('0', 3, "d", timeptr->tm_yday + 1);
			continue;

		case 'm':	/* month, 01 - 12 */
			i = range(0, timeptr->tm_mon, 11);
			FMT('0', 2, "d", i + 1);
			continue;

		case 'M':	/* minute, 00 - 59 */
			i = range(0, timeptr->tm_min, 59);
			FMT('0', 2, "d", i);
			continue;

		case 'p':	/* AM or PM based on 12-hour clock */
		case 'P':	/* am or pm based on 12-hour clock */
			if ((*format == 'p' && (flags & BIT_OF(CHCASE))) ||
			    (*format == 'P' && !(flags & (BIT_OF(CHCASE)|BIT_OF(UPPER))))) {
				flags &= ~(BIT_OF(UPPER)|BIT_OF(CHCASE));
				flags |= BIT_OF(LOWER);
			}
			i = range(0, timeptr->tm_hour, 23);
			if (i < 12)
				tp = ampm[0];
			else
				tp = ampm[1];
			i = 2;
			break;

		case 's':
			FMT('0', 1, "d", (int) ts->tv_sec);
			continue;

		case 'S':	/* second, 00 - 60 */
			i = range(0, timeptr->tm_sec, 60);
			FMT('0', 2, "d", i);
			continue;

		case 'U':	/* week of year, Sunday is first day of week */
			FMT('0', 2, "d", weeknumber(timeptr, 0));
			continue;

		case 'w':	/* weekday, Sunday == 0, 0 - 6 */
			i = range(0, timeptr->tm_wday, 6);
			FMT('0', 1, "d", i);
			continue;

		case 'W':	/* week of year, Monday is first day of week */
			FMT('0', 2, "d", weeknumber(timeptr, 1));
			continue;

		case 'x':	/* appropriate date representation */
			STRFTIME("%m/%d/%y");
			continue;

		case 'X':	/* appropriate time representation */
			STRFTIME("%H:%M:%S");
			continue;

		case 'y':	/* year without a century, 00 - 99 */
			i = timeptr->tm_year % 100;
			FMT('0', 2, "d", i);
			continue;

		case 'Y':	/* year with century */
			FMT('0', 1, "ld", 1900L + timeptr->tm_year);
			continue;

#ifdef MAILHEADER_EXT
		/*
		 * From: Chip Rosenthal <chip@chinacat.unicom.com>
		 * Date: Sun, 19 Mar 1995 00:33:29 -0600 (CST)
		 * 
		 * Warning: the %z [code] is implemented by inspecting the
		 * timezone name conditional compile settings, and
		 * inferring a method to get timezone offsets. I've tried
		 * this code on a couple of machines, but I don't doubt
		 * there is some system out there that won't like it.
		 * Maybe the easiest thing to do would be to bracket this
		 * with an #ifdef that can turn it off. The %z feature
		 * would be an admittedly obscure one that most folks can
		 * live without, but it would be a great help to those of
		 * us that muck around with various message processors.
		 */
 		case 'z':	/* time zone offset east of GMT e.g. -0600 */
			if (precision < 4) precision = 4;
			NEEDS(precision + 1);
			if (gmt) {
				off = 0;
			}
			else {
#ifdef HAVE_TM_NAME
				/*
				 * Systems with tm_name probably have tm_tzadj as
				 * secs west of GMT.  Convert to mins east of GMT.
				 */
				off = -timeptr->tm_tzadj / 60;
#else /* !HAVE_TM_NAME */
#ifdef HAVE_TM_ZONE
				/*
				 * Systems with tm_zone probably have tm_gmtoff as
				 * secs east of GMT.  Convert to mins east of GMT.
				 */
#ifdef RBX_WINDOWS
        off = _timezone / 60;
#else
				off = timeptr->tm_gmtoff / 60;
#endif
#else /* !HAVE_TM_ZONE */
#if HAVE_VAR_TIMEZONE
#if HAVE_VAR_ALTZONE
				off = -(daylight ? timezone : altzone) / 60;
#else
				off = -timezone / 60;
#endif
#else /* !HAVE_VAR_TIMEZONE */
#ifdef HAVE_GETTIMEOFDAY
				gettimeofday(&tv, &zone);
				off = -zone.tz_minuteswest;
#else
				/* no timezone info, then calc by myself */
				{
					struct tm utc;
					time_t now;
					time(&now);
					utc = *gmtime(&now);
					off = (now - mktime(&utc)) / 60;
				}
#endif
#endif /* !HAVE_VAR_TIMEZONE */
#endif /* !HAVE_TM_ZONE */
#endif /* !HAVE_TM_NAME */
			}
			if (off < 0) {
				off = -off;
				*s++ = '-';
			} else {
				*s++ = '+';
			}
			off = off/60*100 + off%60;
			i = snprintf(s, endp - s, (padding == ' ' ? "%*ld" : "%.*ld"),
				     precision - (precision > 4), off);
			if (i < 0) goto err;
			s += i;
			continue;
#endif /* MAILHEADER_EXT */

		case 'Z':	/* time zone name or abbrevation */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(UPPER)|BIT_OF(CHCASE));
				flags |= BIT_OF(LOWER);
			}
			if (gmt) {
				i = 3;
				tp = "UTC";
				break;
			}
#ifdef HAVE_TZNAME
			i = (daylight && timeptr->tm_isdst > 0); /* 0 or 1 */
			tp = tzname[i];
#else
#ifdef HAVE_TM_ZONE
			tp = timeptr->tm_zone;
#else
#ifdef HAVE_TM_NAME
			tp = timeptr->tm_name;
#else
#ifdef HAVE_TIMEZONE
			gettimeofday(& tv, & zone);
#ifdef TIMEZONE_VOID
			tp = timezone();
#else
			tp = timezone(zone.tz_minuteswest, timeptr->tm_isdst > 0);
#endif /* TIMEZONE_VOID */
#endif /* HAVE_TIMEZONE */
#endif /* HAVE_TM_NAME */
#endif /* HAVE_TM_ZONE */
#endif /* HAVE_TZNAME */
			i = strlen(tp);
			break;

#ifdef SYSV_EXT
		case 'n':	/* same as \n */
			FILL_PADDING(1);
			*s++ = '\n';
			continue;

		case 't':	/* same as \t */
			FILL_PADDING(1);
			*s++ = '\t';
			continue;

		case 'D':	/* date as %m/%d/%y */
			STRFTIME("%m/%d/%y");
			continue;

		case 'e':	/* day of month, blank padded */
			FMT(' ', 2, "d", range(1, timeptr->tm_mday, 31));
			continue;

		case 'r':	/* time as %I:%M:%S %p */
			STRFTIME("%I:%M:%S %p");
			continue;

		case 'R':	/* time as %H:%M */
			STRFTIME("%H:%M");
			continue;

		case 'T':	/* time as %H:%M:%S */
			STRFTIME("%H:%M:%S");
			continue;
#endif

#ifdef SUNOS_EXT
		case 'k':	/* hour, 24-hour clock, blank pad */
			i = range(0, timeptr->tm_hour, 23);
			FMT(' ', 2, "d", i);
			continue;

		case 'l':	/* hour, 12-hour clock, 1 - 12, blank pad */
			i = range(0, timeptr->tm_hour, 23);
			if (i == 0)
				i = 12;
			else if (i > 12)
				i -= 12;
			FMT(' ', 2, "d", i);
			continue;
#endif


#ifdef VMS_EXT
		case 'v':	/* date as dd-bbb-YYYY */
			w = snprintf(s, endp - s, "%2d-%3.3s-%4ld",
				     range(1, timeptr->tm_mday, 31),
				     months_l[range(0, timeptr->tm_mon, 11)],
				     timeptr->tm_year + 1900L);
			if (w < 0) goto err;
			for (i = 3; i < 6; i++)
				if (islower(s[i]))
					s[i] = toupper(s[i]);
			s += w;
			continue;
#endif


#ifdef POSIX2_DATE
		case 'C':
			FMT('0', 2, "ld", (timeptr->tm_year + 1900L) / 100);
			continue;


		case 'E':
			/* POSIX locale extensions, ignored for now */
			flags |= BIT_OF(LOCALE_E);
			goto again;
		case 'O':
			/* POSIX locale extensions, ignored for now */
			flags |= BIT_OF(LOCALE_O);
			goto again;

		case 'V':	/* week of year according ISO 8601 */
			FMT('0', 2, "d", iso8601wknum(timeptr));
			continue;

		case 'u':
		/* ISO 8601: Weekday as a decimal number [1 (Monday) - 7] */
			FMT('0', 1, "d", timeptr->tm_wday == 0 ? 7 : timeptr->tm_wday);
			continue;
#endif	/* POSIX2_DATE */

#ifdef ISO_DATE_EXT
		case 'G':
		case 'g':
			/*
			 * Year of ISO week.
			 *
			 * If it's December but the ISO week number is one,
			 * that week is in next year.
			 * If it's January but the ISO week number is 52 or
			 * 53, that week is in last year.
			 * Otherwise, it's this year.
			 */
			w = iso8601wknum(timeptr);
			if (timeptr->tm_mon == 11 && w == 1)
				y = 1900L + timeptr->tm_year + 1;
			else if (timeptr->tm_mon == 0 && w >= 52)
				y = 1900L + timeptr->tm_year - 1;
			else
				y = 1900L + timeptr->tm_year;

			if (*format == 'G')
				FMT('0', 1, "ld", y);
			else
				FMT('0', 2, "ld", y % 100);
			continue;
#endif /* ISO_DATE_EXT */


		case 'L':
			w = 3;
			goto subsec;

		case 'N':
			/*
			 * fractional second digits. default is 9 digits
			 * (nanosecond).
			 *
			 * %3N  millisecond (3 digits)
			 * %6N  microsecond (6 digits)
			 * %9N  nanosecond (9 digits)
			 */
			w = 9;
		subsec:
			{
				long n = ts->tv_nsec;

				if (precision <= 0) {
				    precision = w;
				}
				NEEDS(precision);
				if (precision < w) {
					snprintf(tbuf, w + 1, "%.*ld", w, n);
					memcpy(s, tbuf, precision);
				}
				else {
					snprintf(s, endp - s, "%.*ld", w, n);
					memset(s + w, '0', precision - w);
				}
				s += precision;
			}
			continue;

		case 'F':	/*  Equivalent to %Y-%m-%d */
			{
				int mon, mday;
				mon = range(0, timeptr->tm_mon, 11) + 1;
				mday = range(1, timeptr->tm_mday, 31);
				i = snprintf(s, endp - s, "%ld-%02d-%02d",
					     1900L + timeptr->tm_year, mon, mday);
				if (i < 0)
					goto err;
				s += i;
			}
			continue;

		case '-':
			FLAG_FOUND();
			flags |= BIT_OF(LEFT);
			padding = precision = 0;
			goto again;

		case '^':
			FLAG_FOUND();
			flags |= BIT_OF(UPPER);
			goto again;

		case '#':
			FLAG_FOUND();
			flags |= BIT_OF(CHCASE);
			goto again;

		case '_':
			FLAG_FOUND();
			padding = ' ';
			goto again;

		case '0':
			padding = '0';
		case '1':  case '2': case '3': case '4':
		case '5': case '6':  case '7': case '8': case '9':
			{
				char *e;
				precision = (int)strtoul(format, &e, 10);
				format = e - 1;
				goto again;
			}

		default:
		unknown:
			i = format - sp + 1;
			tp = sp;
			precision = -1;
			flags = 0;
			padding = 0;
			break;
		}
		if (i) {
			FILL_PADDING(i);
			memcpy(s, tp, i);
			switch (flags & (BIT_OF(UPPER)|BIT_OF(LOWER))) {
			case BIT_OF(UPPER):
				do {
					if (islower(*s)) *s = toupper(*s);
				} while (s++, --i);
				break;
			case BIT_OF(LOWER):
				do {
					if (isupper(*s)) *s = tolower(*s);
				} while (s++, --i);
				break;
			default:
				s += i;
				break;
			}
		}
	}
	if (s >= endp) {
		goto err;
	}
	if (*format == '\0') {
		*s = '\0';
		return (s - start);
	} else
		return 0;
}

/* isleap --- is a year a leap year? */

static int isleap(long year) {
	return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}


#ifdef POSIX2_DATE
/* iso8601wknum --- compute week number according to ISO 8601 */

static int iso8601wknum(const struct tm *timeptr) {
	/*
	 * From 1003.2:
	 *	If the week (Monday to Sunday) containing January 1
	 *	has four or more days in the new year, then it is week 1;
	 *	otherwise it is the highest numbered week of the previous
	 *	year (52 or 53), and the next week is week 1.
	 *
	 * ADR: This means if Jan 1 was Monday through Thursday,
	 *	it was week 1, otherwise week 52 or 53.
	 *
	 * XPG4 erroneously included POSIX.2 rationale text in the
	 * main body of the standard. Thus it requires week 53.
	 */

	int weeknum, jan1day;

	/* get week number, Monday as first day of the week */
	weeknum = weeknumber(timeptr, 1);

	/*
	 * With thanks and tip of the hatlo to tml@tik.vtt.fi
	 *
	 * What day of the week does January 1 fall on?
	 * We know that
	 *	(timeptr->tm_yday - jan1.tm_yday) MOD 7 ==
	 *		(timeptr->tm_wday - jan1.tm_wday) MOD 7
	 * and that
	 * 	jan1.tm_yday == 0
	 * and that
	 * 	timeptr->tm_wday MOD 7 == timeptr->tm_wday
	 * from which it follows that. . .
 	 */
	jan1day = timeptr->tm_wday - (timeptr->tm_yday % 7);
	if (jan1day < 0)
		jan1day += 7;

	/*
	 * If Jan 1 was a Monday through Thursday, it was in
	 * week 1.  Otherwise it was last year's highest week, which is
	 * this year's week 0.
	 *
	 * What does that mean?
	 * If Jan 1 was Monday, the week number is exactly right, it can
	 *	never be 0.
	 * If it was Tuesday through Thursday, the weeknumber is one
	 *	less than it should be, so we add one.
	 * Otherwise, Friday, Saturday or Sunday, the week number is
	 * OK, but if it is 0, it needs to be 52 or 53.
	 */
	switch (jan1day) {
	case 1:		/* Monday */
		break;
	case 2:		/* Tuesday */
	case 3:		/* Wednesday */
	case 4:		/* Thursday */
		weeknum++;
		break;
	case 5:		/* Friday */
	case 6:		/* Saturday */
	case 0:		/* Sunday */
		if (weeknum == 0) {
#ifdef USE_BROKEN_XPG4
			/* XPG4 (as of March 1994) says 53 unconditionally */
			weeknum = 53;
#else
			/* get week number of last week of last year */
			struct tm dec31ly;	/* 12/31 last year */
			dec31ly = *timeptr;
			dec31ly.tm_year--;
			dec31ly.tm_mon = 11;
			dec31ly.tm_mday = 31;
			dec31ly.tm_wday = (jan1day == 0) ? 6 : jan1day - 1;
			dec31ly.tm_yday = 364 + isleap(dec31ly.tm_year + 1900L);
			weeknum = iso8601wknum(& dec31ly);
#endif
		}
		break;
	}

	if (timeptr->tm_mon == 11) {
		/*
		 * The last week of the year
		 * can be in week 1 of next year.
		 * Sigh.
		 *
		 * This can only happen if
		 *	M   T  W
		 *	29  30 31
		 *	30  31
		 *	31
		 */
		int wday, mday;

		wday = timeptr->tm_wday;
		mday = timeptr->tm_mday;
		if (   (wday == 1 && (mday >= 29 && mday <= 31))
		    || (wday == 2 && (mday == 30 || mday == 31))
		    || (wday == 3 &&  mday == 31))
			weeknum = 1;
	}

	return weeknum;
}
#endif

/* weeknumber --- figure how many weeks into the year */

/* With thanks and tip of the hatlo to ado@elsie.nci.nih.gov */

static int weeknumber(const struct tm *timeptr, int firstweekday)
{
	int wday = timeptr->tm_wday;
	int ret;

	if (firstweekday == 1) {
		if (wday == 0)	/* sunday */
			wday = 6;
		else
			wday--;
	}
	ret = ((timeptr->tm_yday + 7 - wday) / 7);
	if (ret < 0)
		ret = 0;
	return ret;
}

#if 0
/* ADR --- I'm loathe to mess with ado's code ... */

Date:         Wed, 24 Apr 91 20:54:08 MDT
From: Michal Jaegermann <audfax!emory!vm.ucs.UAlberta.CA!NTOMCZAK>
To: arnold@audiofax.com

Hi Arnold,
in a process of fixing of strftime() in libraries on Atari ST I grabbed
some pieces of code from your own strftime.  When doing that it came
to mind that your weeknumber() function compiles a little bit nicer
in the following form:
/*
 * firstweekday is 0 if starting in Sunday, non-zero if in Monday
 */
{
    return (timeptr->tm_yday - timeptr->tm_wday +
	    (firstweekday ? (timeptr->tm_wday ? 8 : 1) : 7)) / 7;
}
How nicer it depends on a compiler, of course, but always a tiny bit.

   Cheers,
   Michal
   ntomczak@vm.ucs.ualberta.ca
#endif

#ifdef	TEST_STRFTIME

/*
 * NAME:
 *	tst
 *
 * SYNOPSIS:
 *	tst
 *
 * DESCRIPTION:
 *	"tst" is a test driver for the function "strftime".
 *
 * OPTIONS:
 *	None.
 *
 * AUTHOR:
 *	Karl Vogel
 *	Control Data Systems, Inc.
 *	vogelke@c-17igp.wpafb.af.mil
 *
 * BUGS:
 *	None noticed yet.
 *
 * COMPILE:
 *	cc -o tst -DTEST_STRFTIME strftime.c
 */

/* ADR: I reformatted this to my liking, and deleted some unneeded code. */

#ifndef NULL
#include	<stdio.h>
#endif
#include	<sys/time.h>
#include	<string.h>

#define		MAXTIME		132

/*
 * Array of time formats.
 */

static char *array[] =
{
	"(%%A)      full weekday name, var length (Sunday..Saturday)  %A",
	"(%%B)       full month name, var length (January..December)  %B",
	"(%%C)                                               Century  %C",
	"(%%D)                                       date (%%m/%%d/%%y)  %D",
	"(%%E)                           Locale extensions (ignored)  %E",
	"(%%H)                          hour (24-hour clock, 00..23)  %H",
	"(%%I)                          hour (12-hour clock, 01..12)  %I",
	"(%%M)                                       minute (00..59)  %M",
	"(%%O)                           Locale extensions (ignored)  %O",
	"(%%R)                                 time, 24-hour (%%H:%%M)  %R",
	"(%%S)                                       second (00..60)  %S",
	"(%%T)                              time, 24-hour (%%H:%%M:%%S)  %T",
	"(%%U)    week of year, Sunday as first day of week (00..53)  %U",
	"(%%V)                    week of year according to ISO 8601  %V",
	"(%%W)    week of year, Monday as first day of week (00..53)  %W",
	"(%%X)     appropriate locale time representation (%H:%M:%S)  %X",
	"(%%Y)                           year with century (1970...)  %Y",
	"(%%Z) timezone (EDT), or blank if timezone not determinable  %Z",
	"(%%a)          locale's abbreviated weekday name (Sun..Sat)  %a",
	"(%%b)            locale's abbreviated month name (Jan..Dec)  %b",
	"(%%c)           full date (Sat Nov  4 12:02:33 1989)%n%t%t%t  %c",
	"(%%d)                             day of the month (01..31)  %d",
	"(%%e)               day of the month, blank-padded ( 1..31)  %e",
	"(%%h)                                should be same as (%%b)  %h",
	"(%%j)                            day of the year (001..366)  %j",
	"(%%k)               hour, 24-hour clock, blank pad ( 0..23)  %k",
	"(%%l)               hour, 12-hour clock, blank pad ( 0..12)  %l",
	"(%%m)                                        month (01..12)  %m",
	"(%%p)              locale's AM or PM based on 12-hour clock  %p",
	"(%%r)                   time, 12-hour (same as %%I:%%M:%%S %%p)  %r",
	"(%%u) ISO 8601: Weekday as decimal number [1 (Monday) - 7]   %u",
	"(%%v)                                VMS date (dd-bbb-YYYY)  %v",
	"(%%w)                       day of week (0..6, Sunday == 0)  %w",
	"(%%x)                appropriate locale date representation  %x",
	"(%%y)                      last two digits of year (00..99)  %y",
	"(%%z)      timezone offset east of GMT as HHMM (e.g. -0500)  %z",
	(char *) NULL
};

/* main routine. */

int
main(argc, argv)
int argc;
char **argv;
{
	long time();

	char *next;
	char string[MAXTIME];

	int k;
	int length;

	struct tm *tm;

	long clock;

	/* Call the function. */

	clock = time((long *) 0);
	tm = localtime(&clock);

	for (k = 0; next = array[k]; k++) {
		length = strftime(string, MAXTIME, next, tm);
		printf("%s\n", string);
	}

	exit(0);
}
#endif	/* TEST_STRFTIME */
