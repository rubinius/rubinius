#ifndef RBX_LOCKS_SPINLOCK_MUTEX_HPP
#define RBX_LOCKS_SPINLOCK_MUTEX_HPP

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
        reset();
      }

      void reset() {
        flag.clear();
      }

      void lock() {
        while(flag.test_and_set(std::memory_order_seq_cst)) {
          ; // spin
        }
      }

      bool try_lock() {
        return !flag.test_and_set(std::memory_order_seq_cst);
      }

      void unlock() {
        flag.clear(std::memory_order_seq_cst);
        std::atomic_thread_fence(std::memory_order_seq_cst);
      }
    };
  }
}

#endif
