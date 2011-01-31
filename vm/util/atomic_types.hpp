// Don't include directly, include util/atomic.hpp

namespace atomic {
  struct integer {
    uint64_t value;

    integer(uint64_t val)
      : value(val)
    {}

    uint64_t read() {
      atomic::memory_barrier();
      return value;
    }

    void set(uint64_t val) {
      uint64_t tmp = value;
      while(!atomic::compare_and_swap(&value, tmp, val)) {
        tmp = value;
      }
    }

    uint64_t add(uint64_t inc) {
      return atomic::fetch_and_add(&value, inc);
    }

    uint64_t inc() {
      return add(1);
    }

    uint64_t operator++(int) {
      return inc() + 1;
    }

    uint64_t operator++() {
      return inc();
    }

    uint64_t sub(uint64_t dec) {
      return atomic::fetch_and_sub(&value, dec);
    }

    uint64_t dec() {
      return sub(1);
    }
  };
}
