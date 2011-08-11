#include "util/atomic.hpp"

namespace rubinius {
  struct Stats {
    atomic::integer jitted_methods;
    atomic::integer jit_time_spent;

    Stats()
      : jitted_methods(0)
      , jit_time_spent(0)
    {}
  };
}
