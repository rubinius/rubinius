#ifndef RBX_VM_HEAP

#include "builtin/object.hpp"

namespace rubinius {
  // @todo Replace this with the Address class from Immix
  typedef void *address;
  class Object;
  class VM;


  /**
   * A Heap is a chunk of memory used by the Baker GC.
   *
   * Memory allocations are made out of the Heap using bump pointer allocation,
   * until the heap free space is exhausted. To assist in garbage collection,
   * the heap also manages a scan pointer, which identifies the last object
   * visited during a linear scan of the heap.
   */

  class Heap {
    /// Address at which the heap starts
    address start_;

    /// Next free memory in the heap
    address current_;

    /// Address at which the heap ends
    address last_;

    /// Next object to visit in a linear scan of the heap
    address scan_;

    /// A red-line near the end of the heap; once the heap reaches this limit,
    /// a garbage collection will be scheduled.
    address limit_;

    /// Size of the heap in bytes
    size_t size_;

    /// True if this heap is the owner of the underlying memory.
    /// Heaps can be shared; the BakerGC uses a single large heap to allocate
    /// memory, and then creates three smaller heaps within the large heap for
    /// the nursery, heap A and heap B spaces.
    bool owner_;

  public:
    /* Inline methods */

    /**
     * Returns the address at which the next allocation will take place.
     */
    address current() {
      return current_;
    }

    /**
     * Returns the address of the last byte in the heap.
     */
    address last() {
      return last_;
    }

    /**
     * Returns the address of the start of the heap.
     */
    address start() {
      return start_;
    }

    /**
     * Allocates the requested number of +size+ bytes from the heap.
     * Note: This method does not check if the allocation exceeds the heap
     * size, so the caller should have previously checked this.
     *
     * @todo Check that callers use this method in a thread-safe way
     */
    address allocate(size_t size) {
      address addr;
      addr = current_;
      current_ = (address)((uintptr_t)current_ + size);

      return addr;
    }

    /**
     * Returns +size+ bites back to the heap.
     */
    void put_back(size_t size) {
      current_ = (address)((uintptr_t)current_ - size);
    }

    /**
     * True if this heap contains the object at the specified address.
     */
    bool contains_p(address addr) {
      if(addr < start_) return false;
      if(addr >= last_) return false;
      return true;
    }

    // @todo Why do we have two functions that do exactly the same thing?
    bool in_current_p(address addr) {
      if(addr < start_) return false;
      if(addr >= current_) return false;
      return true;
    }

    /**
     * True if the current usage of the heap is at or over the red-line of the
     * limit_ marker.
     */
    bool over_limit_p(void* ptr) {
      return (address)ptr >= limit_;
    }

    /**
     * Returns true if the heap contains enough free space to allocate the
     * specified number of bytes.
     */
    bool enough_space_p(size_t size) {
      if((uintptr_t)current_ + size > (uintptr_t)last_) return false;
      return true;
    }

    /**
     * Attempts to allocate +size+ bytes. If enough free space exists, the
     * allocation succeeds. If insufficient free space is available, the
     * allocation fails and 0 is returned.
     */
    address try_allocate(size_t size) {
      address addr = current_;
      address next = (address)((uintptr_t)current_ + size);
      if(next >= last_) return 0;

      current_ = next;
      return addr;
    }

    /**
     * True if a linear scan of the heap has now visited every object.
     */
    bool fully_scanned_p() {
      return scan_ == current_;
    }

    /**
     * Returns the next object from the current scan location, advancing the
     * scan_ pointer in the process.
     */
    Object* next_unscanned(VM* state) {
      Object* obj;
      if(fully_scanned_p()) return NULL;

      obj = (Object*)scan_;
      scan_ = (address)((uintptr_t)scan_ + obj->size_in_bytes(state));
      return obj;
    }

    /**
     * Returns the first Object in the heap.
     */
    Object* first_object() {
      return (Object*)start_;
    }

    /**
     * Set the scan pointer to +addr+
     */
    void set_scan(address addr) {
      scan_ = addr;
    }

    /**
     * Returns the percentage of the heap that is currently used.
     */
    double percentage_used() {
      size_t used = (size_t)current_ - (size_t)start_;
      return ((double)used/ (double)size_) * 100.0;
    }

    /* Prototypes */
    Heap(size_t size);
    Heap(void* start, size_t size);
    ~Heap();
    void reset();
    size_t remaining();
    size_t used();
    Object* move_object(VM* state, Object*);
  };

}

#endif
