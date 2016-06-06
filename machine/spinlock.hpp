#include <atomic>

namespace rubinius {
  namespace locks {
    // Adapted from: Anthony Williams. “C++ Concurrency In Action.”

    class spinlock_mutex {
      std::atomic_flag flag;
    public:
      spinlock_mutex()
        : flag()
      {
        flag.clear();
      }

      void lock() {
        while(flag.test_and_set(std::memory_order_acquire)) {
          ; // spin
        }
      }

      void unlock() {
        flag.clear(std::memory_order_release);
      }
    };
  }
}
