#include "util/atomic.hpp"

namespace rubinius {
  struct Stats {
    atomic::integer methods_cache_resets;
    atomic::integer jitted_methods;
    atomic::integer jit_time_spent;

    Stats()
      : methods_cache_resets(0)
      , jitted_methods(0)
      , jit_time_spent(0)
    {}
  };
}
