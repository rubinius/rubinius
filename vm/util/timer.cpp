#include "util/timer.hpp"

#include <unistd.h>

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
        ::close(kq_);
      }

      void Timer::set(int milliseconds) {
        if(kq_ == TIMER_INVALID) kq_ = kqueue();

        EV_SET(&filter_, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE,
            NOTE_USECONDS, milliseconds * 1000, NULL);
      }

      void Timer::cancel() {
        if(kq_ != TIMER_INVALID) {
          ::close(kq_);
          kq_ = TIMER_INVALID;
        }
      }

      int Timer::wait_for_tick() {
        struct kevent event;

        return kevent(kq_, &filter_, 1, &event, 1, NULL);
      }

#elif HAVE_TIMERFD
      Timer::Timer() {
      }

      Timer::~Timer() {
      }

      void Timer::set(int interval, int initial) {
      }

      void Timer::cancel() {
      }

      int Timer::wait_for_tick() {
      }

#endif
    }
  }
}
