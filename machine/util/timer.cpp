#include "util/timer.hpp"
#include "logger.hpp"

#include <string.h>
#include <unistd.h>
#include <stdint.h>

#ifdef HAVE_KQUEUE
#include <sys/errno.h>
#endif

#ifdef HAVE_TIMERFD
#include <errno.h>
#include <sys/timerfd.h>
#endif

namespace rubinius {
  namespace utilities {
    namespace timer {
      bool Timer::canceled_p() const {
        return canceled_;
      }

#ifdef HAVE_KQUEUE

      Timer::Timer()
        : canceled_(false)
      {
        if((kq_ = kqueue()) < 0) {
          logger::error("%s: timer: unable to create kqueue", strerror(errno));
        }
      }

      Timer::~Timer() {
        close(kq_);
      }

      void Timer::set(int milliseconds) {
        canceled_ = false;

        EV_SET(&filter_, 0, EVFILT_TIMER, EV_ADD | EV_ENABLE,
            0, milliseconds, NULL);
      }

      void Timer::cancel() {
        canceled_ = true;

        struct kevent event;

        ts_.tv_sec = 0;
        ts_.tv_nsec = 1500000;

        // On macOS, fflags can specify the resolute of the timer, but this is
        // not portable, so it is unset (ie 0), which means milliseconds.
        EV_SET(&filter_, 0, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 1, &ts_);

        kevent(kq_, &filter_, 1, &event, 1, &ts_);
      }

      void Timer::clear() {
        ts_.tv_sec = 0;
        ts_.tv_nsec = 1500000;

        EV_SET(&filter_, 0, EVFILT_TIMER, EV_DELETE, 0, 0, &ts_);

        kevent(kq_, &filter_, 1, NULL, 0, &ts_);
      }

      int Timer::wait_for_tick() {
        struct kevent event;
        int status;

        status = kevent(kq_, &filter_, 1, &event, 1, (struct timespec*)filter_.udata);

        if(status < 0) {
          logger::error("%s: timer: kevent failed", strerror(errno));
          return -1;
        } else if(event.flags & EV_ERROR) {
          logger::error("%s: timer: kevent failed", strerror(event.data));
          return -1;
        } else {
          return event.data;
        }
      }

#elif HAVE_TIMERFD

      Timer::Timer()
        : canceled_(false)
      {
        if((fd_ = timerfd_create(CLOCK_MONOTONIC, 0)) < 0) {
          logger::error("%s: timer: unable to create timer fd", strerror(errno));
        }
      }

      Timer::~Timer() {
        close(fd_);
      }

      void Timer::set(int milliseconds) {
        canceled_ = false;

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
        canceled_ = true;

        struct itimerspec ts;

        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;

        ts.it_value.tv_sec = 0;
        ts.it_value.tv_nsec = 10;

        timerfd_settime(fd_, 0, &ts, NULL);
      }

      void Timer::clear() {
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
