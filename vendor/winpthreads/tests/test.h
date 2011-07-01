/* 
 * test.h
 *
 * Useful definitions and declarations for tests.
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 * 
 *      Contact Email: rpj@callisto.canberra.edu.au
 * 
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 * 
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * 
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 * 
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 */

#ifndef _PTHREAD_TEST_H_
#define _PTHREAD_TEST_H_

#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "../include/pthread.h"
#include "../include/semaphore.h"
#include "../src/misc.h"
#include "../src/mutex.h"
#include "../src/cond.h"
#include "../src/rwlock.h"
#include "../src/thread.h"
#include "../src/spinlock.h"
#include "../src/barrier.h"

#define PTW32_THREAD_NULL_ID {NULL,0}

#include <stdint.h>

const char * error_string[] = {
  "ZERO_or_EOK",
  "EPERM",
  "ENOFILE_or_ENOENT",
  "ESRCH",
  "EINTR",
  "EIO",
  "ENXIO",
  "E2BIG",
  "ENOEXEC",
  "EBADF",
  "ECHILD",
  "EAGAIN",
  "ENOMEM",
  "EACCES",
  "EFAULT",
  "UNKNOWN_15",
  "EBUSY",
  "EEXIST",
  "EXDEV",
  "ENODEV",
  "ENOTDIR",
  "EISDIR",
  "EINVAL",
  "ENFILE",
  "EMFILE",
  "ENOTTY",
  "UNKNOWN_26",
  "EFBIG",
  "ENOSPC",
  "ESPIPE",
  "EROFS",
  "EMLINK",
  "EPIPE",
  "EDOM",
  "ERANGE",
  "UNKNOWN_35",
  "EDEADLOCK_or_EDEADLK",
  "UNKNOWN_37",
  "ENAMETOOLONG",
  "ENOLCK",
  "ENOSYS",
  "ENOTEMPTY",
  "EILSEQ"
};

/*
 * The Mingw32 assert macro calls the CRTDLL _assert function
 * which pops up a dialog. We want to run in batch mode so
 * we define our own assert macro.
 */
#ifdef assert
# undef assert
#endif

#ifndef ASSERT_TRACE
# define ASSERT_TRACE 0
#else
# undef ASSERT_TRACE
# define ASSERT_TRACE 0
#endif

# define assert(e) \
   ((e) ? ((ASSERT_TRACE) ? fprintf(stderr, \
                                    "Assertion succeeded: (%s), file %s, line %d\n", \
			            #e, __FILE__, (int) __LINE__), \
	                            fflush(stderr) : \
                             0) : \
          (fprintf(stderr, "Assertion failed: (%s), file %s, line %d\n", \
                   #e, __FILE__, (int) __LINE__), exit(1), 0))

int assertE;
# define assert_e(e, o, r) \
   (((assertE = e) o (r)) ? ((ASSERT_TRACE) ? fprintf(stderr, \
                                    "Assertion succeeded: (%s), file %s, line %d\n", \
			            #e, __FILE__, (int) __LINE__), \
	                            fflush(stderr) : \
                             0) : \
          (fprintf(stderr, "Assertion failed: (%s %s %s), file %s, line %d, error %s\n", \
                   #e,#o,#r, __FILE__, (int) __LINE__, error_string[assertE]), exit(1), 0))

#ifndef PTW32_VERSION
/* Extensions for winpthread to make more w32 tests happy (non posix): */

/*
 * gmtime(tm) and localtime(tm) return 0 if tm represents
 * a time prior to 1/1/1970.
 */
#define gmtime_r( _clock, _result ) \
        ( gmtime( (_clock) ) \
             ? (*(_result) = *gmtime( (_clock) ), (_result) ) \
             : (0) )

#define localtime_r( _clock, _result ) \
        ( localtime( (_clock) ) \
             ? (*(_result) = *localtime( (_clock) ), (_result) ) \
             : (0) )

#define rand_r( _seed ) \
        ( _seed == _seed? rand() : rand() )

enum ptw32_features {
  PTW32_SYSTEM_INTERLOCKED_COMPARE_EXCHANGE = 0x0001, /* System provides it. */
  PTW32_ALERTABLE_ASYNC_CANCEL              = 0x0002  /* Can cancel blocked threads. */
};

#define pthread_getw32threadhandle_np(self)     ((self).p->h)

/* half-stubbed version */
int pthread_win32_test_features_np(int mask)
{
    int r = 0;

    r = (mask & PTW32_SYSTEM_INTERLOCKED_COMPARE_EXCHANGE); /* assume Win32 */
    /* Not supporting PTW32_ALERTABLE_ASYNC_CANCEL for now, just say it isn't there */
    return r;
}

#endif

#endif
