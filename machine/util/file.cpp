#include "util/file.hpp"

#include <sys/file.h>

namespace rubinius {
  namespace utilities {
    namespace file {
      LockGuard::LockGuard(int fd, int lock_type)
        : fd_(fd)
        , lock_type_(lock_type)
      {
        if(flock(fd_, lock_type_) < 0) {
          status_ = eLockFailed;
        } else {
          status_ = eLockSucceeded;
        }
      }

      LockGuard::~LockGuard() {
        if(status_ == eLockSucceeded) flock(fd_, LOCK_UN);
      }
    }
  }
}
