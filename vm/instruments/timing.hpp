#ifndef RBX_INSTRUMENTS_TIMING_HPP
#define RBX_INSTRUMENTS_TIMING_HPP

#include <stdint.h>
#include <cstdlib>
#include "detection.hpp"

// HACK figure out a better way to detect if we should use
// mach_absolute_time
#if defined(OS_X_ANCIENT) || \
	defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
    __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1050

// @todo: http://developer.apple.com/qa/qa2004/qa1398.html

#include <mach/mach_time.h>
#define get_current_time() mach_absolute_time()
#define TIMING_METHOD "mach_absolute_time"

#elif defined(CLOCK_REALTIME)

#include <time.h>

static inline uint64_t get_current_time() {
  timespec tp;
  if(clock_gettime(CLOCK_REALTIME, &tp)) {
    // error! Do something about it?
    return 0U;
  }
  return (uint64_t)tp.tv_sec * 1000000000UL + tp.tv_nsec;
}

#define TIMING_METHOD "clock_gettime"

#else

#include <sys/time.h>

static inline uint64_t get_current_time() {
  struct timeval tv;
  if(gettimeofday(&tv, NULL) != 0) {
    // Fudge.
    return 0U;
  }

  return (uint64_t)tv.tv_sec * 1000000000UL + tv.tv_usec * 1000L;
}

#define TIMING_METHOD "gettimeofday"

#endif

namespace timer {

  static inline double time_as_double() {
    return (double)get_current_time() * 1000000000;
  }

  template <typename RT, int factor=1>
  class Running {
    RT& result_;
    uint64_t start_;

  public:
    Running(RT& result)
      : result_(result)
    {
      start_ = get_current_time();
    }

    ~Running() {
      result_ += ((get_current_time() - start_) / factor);
    }
  };
}

#endif
