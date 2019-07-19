#include "machine/missing/gettid.h"

#ifndef HAVE_GETTID
#if defined(__APPLE__)
#define _DARWIN_C_SOURCE
#include <pthread.h>
#include <stdint.h>
#elif defined(__NetBSD__)
#include <lwp.h>
#else
#include <sys/syscall.h>
#endif

pid_t gettid(void) {
#if defined(__APPLE__)
  uint64_t tid;
  pthread_threadid_np(NULL, &tid);
  return (pid_t)tid;
#elif defined(__FreeBSD__)
  long tid;
  syscall(SYS_thr_self, &tid);
  return (pid_t)(tid);
#elif defined(__NetBSD__)
  return (pid_t)_lwp_self();
#elif defined(SYS_gettid)
  return (pid_t)syscall(SYS_gettid);
#else
  return 0;
#endif
}
#endif
