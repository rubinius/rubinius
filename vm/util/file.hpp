#ifndef RBX_UTIL_FILE_HPP
#define RBX_UTIL_FILE_HPP

namespace rubinius {
  namespace utilities {
    namespace file {
      enum LockStatus {
        eLockFailed,
        eLockSucceeded
      };

      class LockGuard {
        int fd_;
        int lock_type_;
        LockStatus status_;

      public:

        LockGuard(int fd, int lock_type);
        ~LockGuard();

        int fd() { return fd_; }
        int lock_type() { return lock_type_; }
        LockStatus status() { return status_; }
      };
    }
  }
}
#endif
