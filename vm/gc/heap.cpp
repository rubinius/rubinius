#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "gc/heap.hpp"
#include "util/gc_alloc.hpp"

namespace rubinius {

  /**
   * Allocates a new chunk of memory of +bytes+ size.
   */
  Heap::Heap(size_t bytes)
    : size_(bytes)
    , owner_(true)
  {
    start_ = Address(memory::gc_alloc(size_));
    last_ = start_ + bytes - 1;

    int red_zone = bytes / 1024;
    limit_ = last_ - red_zone;

    reset();
  }

  /**
   * Allocates a new Heap of +bytes+ size at the specified address.
   * The new Heap is not the owner of the underlying memory, and will not
   * free it on destruction.
   */
  Heap::Heap(Address start, size_t bytes)
    : start_(start)
    , size_(bytes)
    , owner_(false)
  {
    last_ = start_ + bytes - 1;

    int red_zone = bytes / 1024;
    limit_ = last_ - red_zone;

    reset();
  }

  /**
   * Destroys the Heap; if the underlying memory is owned by this object,
   * it is freed.
   */
  Heap::~Heap() {
    if(owner_) {
      memory::gc_free(start_, size_);
    }
  }

  /**
   * Resets the Heap bump allocation pointer, making the entire heap memory
   * available again for allocations.
   *
   * Note: The memory is not zeroed.
   */
  void Heap::reset() {
    current_ = start_;
    scan_ = start_;
#ifdef RBX_GC_DEBUG
// To heavy when doing GC stress and not necessary
#ifndef RBX_GC_STRESS
    memset(start_, 0xff, size_);
#endif
#endif
  }

  /**
   * Moves +orig+ Object into this Heap, and sets a forwarding pointer to the
   * new location.
   *
   * @returns the new location of +orig+.
   */
  Object* Heap::move_object(VM* state, Object* orig) {
    size_t bytes = orig->size_in_bytes(state);
    Object* tmp = allocate(bytes).as<Object>();

    memcpy(tmp, orig, bytes);
    orig->set_forward(tmp);

    return tmp;
  }

}
