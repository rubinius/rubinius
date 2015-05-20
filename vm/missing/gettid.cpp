#include "vm/missing/gettid.h"

#ifndef HAVE_GETTID
#include <sys/syscall.h>

pid_t gettid(void) {
#if defined(__APPLE__)
  return (pid_t)syscall(SYS_thread_selfid);
#elif defined(__FreeBSD__)
  long tid;
  syscall(SYS_thr_self, &tid);
  return (pid_t)(tid);
#elif defined(SYS_gettid)
  return (pid_t)syscall(SYS_gettid);
#else
  return 0;
#endif
}
#endif
