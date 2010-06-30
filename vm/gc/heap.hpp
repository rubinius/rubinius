#ifndef RBX_VM_HEAP

#include "builtin/object.hpp"

namespace rubinius {
  typedef void *address;
  class Object;
  class VM;

  class Heap {
    address start_;
    address current_;
    address last_;
    address scan_;
    address limit_;

    size_t size_;
    bool owner_;

  public:
    /* Inline methods */
    address current() {
      return current_;
    }

    address last() {
      return last_;
    }

    address start() {
      return start_;
    }

    address allocate(size_t size) {
      address addr;
      addr = current_;
      current_ = (address)((uintptr_t)current_ + size);

      return addr;
    }

    void put_back(size_t size) {
      current_ = (address)((uintptr_t)current_ - size);
    }

    bool contains_p(address addr) {
      if(addr < start_) return false;
      if(addr >= last_) return false;
      return true;
    }

    bool in_current_p(address addr) {
      if(addr < start_) return false;
      if(addr >= current_) return false;
      return true;
    }

    bool over_limit_p(void* ptr) {
      return (address)ptr >= limit_;
    }

    bool enough_space_p(size_t size) {
      if((uintptr_t)current_ + size > (uintptr_t)last_) return false;
      return true;
    }

    address try_allocate(size_t size) {
      address addr = current_;
      address next = (address)((uintptr_t)current_ + size);
      if(next >= last_) return 0;

      current_ = next;
      return addr;
    }

    bool fully_scanned_p() {
      return scan_ == current_;
    }

    Object* next_unscanned(VM* state) {
      Object* obj;
      if(fully_scanned_p()) return NULL;

      obj = (Object*)scan_;
      scan_ = (address)((uintptr_t)scan_ + obj->size_in_bytes(state));
      return obj;
    }

    Object* first_object() {
      return (Object*)start_;
    }

    // Set the scan pointer to +addr+
    void set_scan(address addr) {
      scan_ = addr;
    }

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
    Object* copy_object(VM* state, Object*);
  };

}

#endif
