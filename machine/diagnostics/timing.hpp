#ifndef RBX_DIAGNOSTICS_TIMING_HPP
#define RBX_DIAGNOSTICS_TIMING_HPP

#include <stdint.h>
#include <stdlib.h>
#include <atomic>

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

#elif defined(CLOCK_MONOTONIC)

#include <time.h>

static inline uint64_t get_current_time() {
  timespec tp;
  if(clock_gettime(CLOCK_MONOTONIC, &tp)) {
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

  const int nanoseconds = 1;
  const int microseconds = 1000;
  const int milliseconds = 1000000;
  const int seconds = 1000000000;

  static inline double time_elapsed_seconds(uint64_t start_time) {
    return (double)(get_current_time() - start_time) / seconds;
  }

  static inline double time_as_double() {
    return (double)get_current_time() * 1000000000;
  }

  static inline double elapsed_seconds(uint64_t nanoseconds) {
    return (double)nanoseconds / seconds;
  }

  template <int factor=milliseconds>
  class StopWatch {
    uint64_t& total_;
    uint64_t start_;

  public:
    StopWatch(uint64_t& total)
      : total_(total)
    {
      start_ = get_current_time();
    }

    ~StopWatch() {
      uint64_t now = get_current_time();
      total_ += (now - start_) / ((uint64_t)factor);
    }
  };

  template <int factor=1>
  class Running {
    std::atomic<uint64_t>& result_;
    std::atomic<uint64_t>* last_;
    uint64_t start_;

  public:
    Running(std::atomic<uint64_t>& result)
      : result_(result)
      , last_(0)
    {
      start_ = get_current_time();
    }

    Running(std::atomic<uint64_t>& result, std::atomic<uint64_t>& last)
      : result_(result)
      , last_(&last)
    {
      start_ = get_current_time();
    }

    ~Running() {
      uint64_t now = get_current_time();
      uint64_t run = (now - start_) / ((uint64_t)factor);
      if(last_) *last_ = run;

      result_ += run;
    }
  };
}

#endif
