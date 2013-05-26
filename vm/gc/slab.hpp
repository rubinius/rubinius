#ifndef RBX_GC_SLAB_HPP
#define RBX_GC_SLAB_HPP

#include "util/address.hpp"

using memory::Address;

#include <stdint.h>
#include <unistd.h>

namespace rubinius {
namespace gc {

  /***
   * Represents a contiguous range of memory from which new Objects can be
   * allocated. Slabs are allocated from the Eden space in the young generation,
   * and used as thread-local allocation pools. This minimises the need for
   * locking when creating new objects in a multi-threaded application.
   */

  class Slab {
    /// Size of the memory slab
    size_t size_;

    /// Starting address of the memory slab
    Address start_;

    /// Address of the last byte of the memory slab
    Address end_;

    /// Current position to allocate from
    Address current_;

    /// The number of allocations done
    size_t allocations_;

  public:

    size_t allocations() const {
      return allocations_;
    }

    size_t byte_used() const {
      return current_ - start_;
    }

    /**
     * Refills the slab, by pointing it to a new memory region.
     * Slabs are refilled when they are full, or when a young generation
     * garbage collection has been performed.
     */
    void refill(Address start, size_t size) {
      allocations_ = 0;
      size_ = size;
      start_ = start;
      end_ = start_ + size_;
      current_ = start_;
    }

    Slab(Address start, size_t size)
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

    /**
     * Returns the Address of a memory allocation of the +bytes+ size.
     * If insufficient free space is available, the allocation fails and 0
     * is returned.
     */
    Address allocate(size_t bytes) {
      Address addr = current_;
      Address new_current = addr + bytes;

      if(new_current >= end_) return 0;

      ++allocations_;
      current_ = new_current;

      return addr;
    }
  };
}}

#endif
