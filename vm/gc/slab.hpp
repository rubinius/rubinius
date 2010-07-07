#ifndef RBX_GC_SLAB_HPP
#define RBX_GC_SLAB_HPP

namespace rubinius {
namespace gc {
  class Slab {
    size_t size_;
    uintptr_t start_;
    uintptr_t end_;

    // Current position to allocate from
    uintptr_t current_;

    // The number of allocations done
    size_t allocations_;

  public:

    size_t allocations() {
      return allocations_;
    }

    size_t byte_used() {
      return current_ - start_;
    }

    void refill(void* start, size_t size) {
      size_ = size;
      start_ = reinterpret_cast<uintptr_t>(start);
      end_ = start_ + size_;
      current_ = start_;
    }

    Slab(void* start, size_t size)
      : allocations_(0)
    {
      refill(start, size);
    }

    Slab()
      : size_(0)
      , start_(0)
      , end_(0)
      , current_(0)
      , allocations_(0)
    {}

    void* allocate(size_t bytes) {
      uintptr_t addr = current_;
      uintptr_t new_current = addr + bytes;

      if(new_current >= end_) return 0;

      ++allocations_;
      current_ = new_current;

      return reinterpret_cast<void*>(addr);
    }
  };
}}

#endif
