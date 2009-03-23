#ifndef RBX_GLOBAL_LOCK_HPP
#define RBX_GLOBAL_LOCK_HPP

#include "util/thread.hpp"

namespace rubinius {
  typedef thread::Mutex GlobalLock;
}

#endif
