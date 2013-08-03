/*
 * This software is derived from the software developed by Michael G Schwern
 * and is available at http://github.com/mschwern/y2038. That software is
 * originally derived from Paul Sheer's pivotal_gmtime_r.c.
 * The code has been modified by Dirkjan Bussink to incorporate several concepts
 * taken from Ruby 1.9.3, regarding the mapping tables for years far in the future
 * and far in the past.
 */

/*
 * Original license of y2038:
 *
 * The MIT License
 *
 * Copyright (c) 2007-2010 Michael G Schwern
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * Ruby is dual licensed with one of the licenses being the BSD license:
 *
 * Copyright (C) 1993-2010 Yukihiro Matsumoto. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "time64.h"

/* Prevent NetBSD from using ctype macros that cause compiler
 * warnings about char as subscript.
 */
#define _CTYPE_NOINLINE 1

/* Number of days in each month. Used for translating
 * a timestamp in a struct tm.
 */
static const int days_in_month[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

/* Length of the year. Needed for translating
 * a timestamp into a struct tm
 */
static const int length_of_year[2] = { 365, 366 };

/* Number of julian days passed for a given month.
 * Used for computing the day of the year from
 * a timestamp
 */
static const int julian_days_by_month[2][12] = {
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
};

/* Translation table for years out of the standard
 * range. This is used for translating a struct tm
 * into a timestamp. The translation table is used
 * so we assume the oldest timezone settings for
 * dates out of range in the past.
 *
 * The entries here are years that for the given month
 * and day of the month return a year that has the
 * same day in the same month. This means that that
 * year would have the same DST transfer as the
 * year outside of the range.
 *
 * These tables are only used when the standard
 * system functions fail.
 */
static const int lower_common_month_table[12][7] = {
  { 1905, 1906, 1907, 1902, 1903, 1904, 1910,},
  { 1903, 1909, 1910, 1905, 1906, 1907, 1902,},
  { 1903, 1909, 1904, 1905, 1906, 1907, 1902,},
  { 1906, 1907, 1902, 1903, 1909, 1904, 1905,},
  { 1904, 1905, 1906, 1907, 1902, 1903, 1909,},
  { 1902, 1903, 1909, 1904, 1905, 1906, 1907,},
  { 1906, 1907, 1902, 1903, 1909, 1904, 1905,},
  { 1909, 1904, 1905, 1906, 1907, 1902, 1903,},
  { 1907, 1902, 1903, 1909, 1904, 1905, 1906,},
  { 1905, 1906, 1907, 1902, 1903, 1909, 1904,},
  { 1903, 1909, 1904, 1905, 1906, 1907, 1902,},
  { 1907, 1902, 1903, 1909, 1904, 1905, 1906,},
};

static const int lower_leap_month_table[7] = {
  1920, 1904, 1916, 1928, 1912, 1924, 1908,
};


/* Translation table for years out of the standard
 * range. This is used for translating a struct tm
 * into a timestamp. The translation table is used
 * so we assume the newest timezone settings for
 * dates out of range in the future.
 *
 * These tables are only used when the standard
 * system functions fail.*
 */
static const int higher_common_month_table[12][7] = {
  /* Sun   Mon   Tue   Wed   Thu   Fri   Sat */
  { 2034, 2035, 2036, 2031, 2032, 2027, 2033 }, /* January */
  { 2026, 2027, 2033, 2034, 2035, 2030, 2031 }, /* February */
  { 2026, 2032, 2033, 2034, 2035, 2030, 2036 }, /* March */
  { 2035, 2030, 2036, 2026, 2032, 2033, 2034 }, /* April */
  { 2033, 2034, 2035, 2030, 2036, 2026, 2032 }, /* May */
  { 2036, 2026, 2032, 2033, 2034, 2035, 2030 }, /* June */
  { 2035, 2030, 2036, 2026, 2032, 2033, 2034 }, /* July */
  { 2032, 2033, 2034, 2035, 2030, 2036, 2026 }, /* August */
  { 2030, 2036, 2026, 2032, 2033, 2034, 2035 }, /* September */
  { 2034, 2035, 2030, 2036, 2026, 2032, 2033 }, /* October */
  { 2026, 2032, 2033, 2034, 2035, 2030, 2036 }, /* November */
  { 2030, 2036, 2026, 2032, 2033, 2034, 2035 }, /* December */
};

static const int higher_leap_month_table[7] = {
/* Sun   Mon   Tue   Wed   Thu   Fri   Sat */
  2032, 2016, 2028, 2012, 2024, 2036, 2020, /* February */
};

/* Utility define for making sure we wrap parts of
 * a struct tm for negative timestamps.
 */
#define WRAP(a,b,m)     ((a) = ((a) <  0  ) ? ((b)--, (a) + (m)) : (a))

/* One gregorian cycle is 400 years. So the number of days
 * is 400 standard years, plus 100 leap days minus 3 because
 * years % 100 but not % 400 are no leap year
 */
#define DAYS_IN_GREGORIAN_CYCLE (365 * 400 + 100 - 3)
static const time64_t seconds_in_gregorian_cycle = DAYS_IN_GREGORIAN_CYCLE * 86400LL;

/* Copy a struct tm64 into a regular struct tm. This is
 * done before passing it into the standard system functions
 */
int tm64_to_tm(struct tm64* tm64, struct tm* tm) {
  tm->tm_sec    = tm64->tm_sec;
  tm->tm_min    = tm64->tm_min;
  tm->tm_hour   = tm64->tm_hour;
  tm->tm_mday   = tm64->tm_mday;
  tm->tm_mon    = tm64->tm_mon;
  tm->tm_wday   = tm64->tm_wday;
  tm->tm_yday   = tm64->tm_yday;
  tm->tm_isdst  = tm64->tm_isdst;
#ifdef HAVE_TM_GMTOFF
  tm->tm_gmtoff = tm64->tm_gmtoff;
#endif
#ifdef HAVE_TM_ZONE
  tm->tm_zone   = tm64->tm_zone;
#endif

  int64_t year = tm64->tm_year - 1900;
  if(year > INT_MAX || year < INT_MIN) {
    tm->tm_year = 0;
    return -1;
  }
  tm->tm_year = (int)year;
  return 0;
}

/* Copy a regular struct tm into a struct tm64. This is
 * done after passing it into the standard system functions
 */
int tm_to_tm64(struct tm* tm, struct tm64* tm64) {
  tm64->tm_sec    = tm->tm_sec;
  tm64->tm_min    = tm->tm_min;
  tm64->tm_hour   = tm->tm_hour;
  tm64->tm_mday   = tm->tm_mday;
  tm64->tm_mon    = tm->tm_mon;
  tm64->tm_wday   = tm->tm_wday;
  tm64->tm_yday   = tm->tm_yday;
  tm64->tm_isdst  = tm->tm_isdst;
#ifdef HAVE_TM_GMTOFF
  tm64->tm_gmtoff = tm->tm_gmtoff;
#else
  tm64->tm_gmtoff = 0;
#endif
#ifdef HAVE_TM_ZONE
  tm64->tm_zone   = (char*)tm->tm_zone;
#elif HAVE_TZNAME && HAVE_DAYLIGHT
  tm64->tm_zone   = tzname[daylight && tm->tm_isdst];
#else
  tm64->tm_zone   = NULL;
#endif

  tm64->tm_year   = tm->tm_year + 1900;
  return 0;
}

/* Compute the day of the week for a date. This is the
 * Sakamoto algorithm
 */

static const int month_offsets[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
int day_of_week(int64_t y, int m, int d) {
   y -= m < 2;
   return (y + y/4 - y/100 + y/400 + month_offsets[m] + d) % 7;
}

/* Whether the given year is a leap year or not.
 */
int leap_year(int64_t n) {
  return !( n % 400 && (n % 4 || (n % 100 == 0)));
}

static int day_before_leap(struct tm64* tm64) {
  return (tm64->tm_mon == 0) || (tm64->tm_mon == 1 && tm64->tm_mday < 29);
}

/* Translate a number of years in difference into
 * seconds. This is used for translating back into
 * a timestamp after using the conversion table for
 * computing with a compatible date.
 */
time64_t year_diff_to_seconds(int64_t from, int64_t to, int pre_leap_day) {
  int increment = (from < to) ? 1 : -1;
  time64_t seconds = 0;
  int64_t cycles;

  /*
   * We can assume here that from is in a range close to 1970,
   * since it was used to compute the struct tm. Therefore
   * we can just look at how much complete gregorian cycles the to date
   * has after 2038 of before 1901.
   */
  if(to > 2400) {
    cycles   = (to - 2400) / 400;
    to      -= cycles * 400;
    seconds += cycles * seconds_in_gregorian_cycle;
  } else if(to < 1600) {
    cycles   = (to - 1600) / 400;
    to      += cycles * 400;
    seconds += cycles * seconds_in_gregorian_cycle;
  }

  int traversal_includes_leap = increment < 1 ? !pre_leap_day : pre_leap_day;
  while( to != from) {
    int64_t next_year = to - increment;
    int64_t year_to_count = traversal_includes_leap ? next_year : to;
    seconds += length_of_year[leap_year(year_to_count)] * 86400LL;
    to = next_year;
  }

  return seconds * increment;
}

time64_t timestamp64_mktime(time_t (*func)(struct tm*), struct tm* tm) {
  int orig_tm_isdst;
  time_t time;

  time = func(tm);
  if(time == -1 && tm->tm_isdst != 0 && (errno == EINVAL || errno == EOVERFLOW)) {
    /*
     * Retry one time because of perhaps DST change or inappropriate value of tm_isdst.
     * The invalid time due to DST change only happens if time moves forward (thus creating the DST gap),
     * so we know we start off with DST not active. This happens at least on NetBSD with tm_isdst == -1.
     * Some implementations of mktime() return -1 with errno of EOVERFLOW if isdst flag is set to 1 and the time
     * point is not in a DST period. Observed on NetBSD.
     */
    orig_tm_isdst = tm->tm_isdst;
    tm->tm_isdst = 0;
    time = func(tm);
    if(time == -1 && (errno == EINVAL || errno == EOVERFLOW)) {
      /* Restore the original value, adjusting tm_isdst didn't help. */
      tm->tm_isdst = orig_tm_isdst;
    }
  }
  return (time64_t) time;
}

time64_t timestamp64(time_t (*func)(struct tm*), struct tm64* tm64) {
  time64_t time;
  struct tm tm;
  memset(&tm, 0, sizeof(tm));

  /* If this succeeds it fits in a standard struct tm */
  if(tm64_to_tm(tm64, &tm) == 0) {
    time = timestamp64_mktime(func, &tm);
    /*
     * This still would fail for 1969-12-31 23:59:59 GMT, but
     * the fallback code will properly handle that case anyway.
     */
    if(time != -1) {
      /* Copy back updated information */
      tm_to_tm64(&tm, tm64);
      return time;
    }
  }

  /*
   * Convert the struct to a year that is day compatible with
   * the current day.
   */

  int64_t year = tm64->tm_year;

  if(year < 1902) {
    /* For years below the 32 bit size time_t value we need to use
    * the lower comparable years */
    int day = day_of_week(year, tm64->tm_mon, 1);
    if(tm64->tm_mon == 2 && leap_year(year)) {
      tm.tm_year = lower_leap_month_table[day] - 1900;
    } else {
      tm.tm_year = lower_common_month_table[tm.tm_mon][day] - 1900;
    }
  } else if(year > 2037) {
    /* For years above the 32 bit size time_t value we need to use
     * the lower comparable years */
    int day = day_of_week(year, tm64->tm_mon, 1);
    if(tm64->tm_mon == 2 && leap_year(year)) {
      tm.tm_year = higher_leap_month_table[day] - 1900;
    } else {
      tm.tm_year = higher_common_month_table[tm.tm_mon][day] - 1900;
    }
  }

  time = timestamp64_mktime(func, &tm);
  tm_to_tm64(&tm, tm64);

  if(year != tm64->tm_year) {
    /* Correct for the changed year to do the mktime computation */
    time += year_diff_to_seconds(tm64->tm_year, year, day_before_leap(tm64));
  }

  tm64->tm_year = year;
  return time;
}

time64_t mktime64(struct tm64* tm64) {
  return timestamp64(mktime, tm64);
}

time64_t timelocal64(struct tm64* tm64) {
#if defined( __OpenBSD__) || defined(__FreeBSD__)
  return timestamp64(timelocal, tm64);
#else
  return timestamp64(mktime, tm64);
#endif
}

time64_t timegm64(struct tm64* tm64) {
  return timestamp64(timegm, tm64);
}


struct tm64* gmtime64_r(const time64_t* time64, struct tm64* tm64) {

  if(*time64 >= LONG_MIN && *time64 <= LONG_MAX) {
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    time_t time = (time_t) *time64;

    if(gmtime_r(&time, &tm)) {
      tm_to_tm64(&tm, tm64);
      /* Always set to UTC even if the platform returns GMT (eg: Linux) */
      tm64->tm_zone = (char *)"UTC";
      return tm64;
    }
  }

  tm64->tm_isdst = 0;
  tm64->tm_gmtoff = 0;
  tm64->tm_zone = (char *)"UTC";

  /*
   * Fallback because out of range of time_t or gmtime_r failed
   */
  int64_t year = 1970;
  int leap = 0;

  time64_t days = *time64;
  tm64->tm_sec = (int)(days % 60);
  days /= 60;
  tm64->tm_min = (int)(days % 60);
  days /= 60;
  tm64->tm_hour = (int)(days % 24);
  days /= 24;

  WRAP (tm64->tm_sec, tm64->tm_min, 60);
  WRAP (tm64->tm_min, tm64->tm_hour, 60);
  WRAP (tm64->tm_hour, days, 24);

  /*
   * days now contains the number of days that have passed.
   * Week day is easy since we know the day that the epoch occured
   */
  tm64->tm_wday = (int)((days + 4) % 7);
  if(tm64->tm_wday < 0) {
    tm64->tm_wday += 7;
  }

  if(days >= 0) {
    /* Gregorian cycles */
    int64_t cycles = days / DAYS_IN_GREGORIAN_CYCLE;
    if(cycles) {
      days -= cycles * DAYS_IN_GREGORIAN_CYCLE;
      year += cycles * 400;
    }

    leap = leap_year(year);
    while (days >= length_of_year[leap]) {
      days -= length_of_year[leap];
      ++year;
      leap = leap_year(year);
    }

    tm64->tm_mon = 0;
    while (days >= days_in_month[leap][tm64->tm_mon]) {
      days -= days_in_month[leap][tm64->tm_mon];
      tm64->tm_mon++;
    }
  } else {
    --year;

    int64_t cycles = days / DAYS_IN_GREGORIAN_CYCLE + 1;
    if(cycles) {
      days -= cycles * DAYS_IN_GREGORIAN_CYCLE;
      year += cycles * 400;
    }

    leap = leap_year(year);
    while (days < -length_of_year[leap]) {
      days += length_of_year[leap];
      year--;
      leap = leap_year(year);
    }

    /* Months */
    tm64->tm_mon = 11;
    while (days < -days_in_month[leap][tm64->tm_mon]) {
      days += days_in_month[leap][tm64->tm_mon];
      tm64->tm_mon--;
    }
    days += days_in_month[leap][tm64->tm_mon];
  }

  tm64->tm_year = year;
  /* Remaining days are the day of the month, +1 since we don't
   * count with offset 0 for days */
  tm64->tm_mday = (int) days + 1;
  tm64->tm_yday = julian_days_by_month[leap][tm64->tm_mon] + (int)days;

  return tm64;
}

struct tm64* localtime64_r(const time64_t* time64, struct tm64* tm64) {
  if(*time64 >= LONG_MIN && *time64 <= LONG_MAX) {
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    time_t time = (time_t) *time64;

    if(localtime_r(&time, &tm)) {
      tm_to_tm64(&tm, tm64);
#ifndef HAVE_TM_ZONE
#if HAVE_TZNAME && HAVE_DAYLIGHT
      tm64->tm_zone = tzname[daylight && tm64->tm_isdst];
#else
      tm64->tm_zone = NULL;
#endif
#endif
      return tm64;
    }
  }

  /*
   * First determine the (approximate) year that this timestamp
   * is in using the gmtime64_r function. We then use this so
   * we are able to map this to a year that should be compatible
   * with the system localtime_r
   */

  struct tm64 gm;
  gmtime64_r(time64, &gm);

  int64_t gm_year = gm.tm_year;

  if(gm_year < 1902) {
    /* For years below the 32 bit size time_t value we need to use
    * the lower comparable years */
    int day = day_of_week(gm_year, gm.tm_mon, 1);
    if(gm.tm_mon == 2 && leap_year(gm_year)) {
      gm.tm_year = lower_leap_month_table[day];
    } else {
      gm.tm_year = lower_common_month_table[gm.tm_mon][day];
    }
  } else if(gm_year > 2037) {
    /* For years above the 32 bit size time_t value we need to use
     * the lower comparable years */
    int day = day_of_week(gm_year, gm.tm_mon, 1);
    if(gm.tm_mon == 2 && leap_year(gm_year)) {
      gm.tm_year = higher_leap_month_table[day];
    } else {
      gm.tm_year = higher_common_month_table[gm.tm_mon][day];
    }
  }

  /*
   * Get the timestamp for the safe date that we mapped the
   * large date to.
   */
  time_t time = (time_t)timegm64(&gm);
  struct tm tm;
  memset(&tm, 0, sizeof(tm));

  localtime_r(&time, &tm);
  tm_to_tm64(&tm, tm64);
  tm64->tm_year = gm_year;

  /*
   * We need to correct here for the case where the timezone difference
   * results in the dates occuring in two different years.
   */
  int month_diff = tm64->tm_mon - gm.tm_mon;
  if(month_diff == 11) {
    tm64->tm_year--;
  } else if(month_diff == -11) {
    tm64->tm_year++;
  }

  /* If the gm year is a leap year, but the local isn't and we're on
   * December 31st in the local year, this is marked as day 365 because
   * the gm year is a leap year. Therefore we need to substract 1.
   */
  if(!leap_year(tm64->tm_year) && tm64->tm_yday == 365 ) {
    tm64->tm_yday--;
  }

#ifndef HAVE_TM_ZONE
#if HAVE_TZNAME && HAVE_DAYLIGHT
  tm64->tm_zone = tzname[daylight && tm64->tm_isdst];
#else
  tm64->tm_zone = NULL;
#endif
#endif

  return tm64;
}
