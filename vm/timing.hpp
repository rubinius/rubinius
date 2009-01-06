#ifndef RBX_TIMING_HPP

// HACK figure out a better way to detect if we should use
// mach_absolute_time
#if defined(OS_X_ANCIENT) || \
	defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050

#include <mach/mach_time.h>
#define get_current_time() mach_absolute_time()

#else

#include <time.h>

uint64_t get_current_time() {
  timespec tp;
  if(clock_gettime(CLOCK_REALTIME, &tp)) {
    // error! Do something about it?
    return 0U;
  }
  return tp.tv_sec * 1000000000UL + tp.tv_nsec;
}

#endif

#endif
