#include "util/timer.hpp"
#include "util/logger.hpp"

#include <string.h>
#include <unistd.h>
#include <stdint.h>

#ifdef HAVE_TIMERFD
#include <errno.h>
#include <sys/timerfd.h>
#endif

namespace rubinius {
  namespace utilities {
    namespace timer {
#ifdef HAVE_KQUEUE

#define TIMER_INVALID     (-1)

      Timer::Timer()
        : kq_(TIMER_INVALID)
      {
      }

      Timer::~Timer() {
        close(kq_);
      }

      void Timer::set(int milliseconds) {
        if(kq_ == TIMER_INVALID) {
          if((kq_ = kqueue() < 0)) {
            logger::error("%s: timer: unable to create kqueue", strerror(errno));
            return;
          }
        }

        EV_SET(&filter_, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE,
            NOTE_USECONDS, milliseconds * 1000, NULL);
      }

      void Timer::cancel() {
        if(kq_ != TIMER_INVALID) {
          close(kq_);
          kq_ = TIMER_INVALID;
        }
      }

      int Timer::wait_for_tick() {
        if(kq_ != TIMER_INVALID) {
          struct kevent event;

          return kevent(kq_, &filter_, 1, &event, 1, NULL);
        }
      }

#elif HAVE_TIMERFD

      Timer::Timer() {
        if((fd_ = timerfd_create(CLOCK_MONOTONIC, 0)) < 0) {
          logger::error("%s: timer: unable to create timer fd", strerror(errno));
        }
      }

      Timer::~Timer() {
        close(fd_);
      }

      void Timer::set(int milliseconds) {
        struct itimerspec ts;

        if(milliseconds > 1000) {
	  ts.it_interval.tv_sec = milliseconds / 1000;
          ts.it_interval.tv_nsec = (milliseconds % 1000) * 1000000;
        } else {
          ts.it_interval.tv_sec = 0;
          ts.it_interval.tv_nsec = milliseconds * 1000000;
        }

        ts.it_value = ts.it_interval;

        timerfd_settime(fd_, 0, &ts, NULL);
      }

      void Timer::cancel() {
        struct itimerspec ts;

        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;

        ts.it_value.tv_sec = 0;
        ts.it_value.tv_nsec = 10;

        timerfd_settime(fd_, 0, &ts, NULL);
      }

      int Timer::wait_for_tick() {
        uint64_t ticks = 0;

        if(read(fd_, &ticks, sizeof(uint64_t)) != sizeof(uint64_t)) {
          logger::error("%s: timer: failed to read ticks", strerror(errno));
        }

        return ticks;
      }

#endif
    }
  }
}
