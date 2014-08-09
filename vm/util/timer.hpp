#ifndef RBX_UTIL_TIMER_HPP
#define RBX_UTIL_TIMER_HPP

#ifdef HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#elif HAVE_TIMERFD
#else
#error "No timer utility is available. Please submit an issue."
#endif

namespace rubinius {
  namespace utilities {
    namespace timer {
      class Timer {

#ifdef HAVE_KQUEUE
        int kq_;
        struct kevent filter_;
        struct timespec ts_;
#elif HAVE_TIMERFD
        int fd_;
#endif

      public:
        Timer();
        virtual ~Timer();

        void set(int milliseconds);
        void cancel();
        void clear();
        int wait_for_tick();
      };
    }
  }
}

#endif
