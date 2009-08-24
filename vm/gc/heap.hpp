#ifndef RBX_VM_HEAP

#include "builtin/object.hpp"

namespace rubinius {
  typedef void *address;
  class Object;
  class VM;

  class Heap {
    /* Fields */

  public:
    address start;
    address current;
    address last;
    address scan;

    size_t size;

    /* Inline methods */

    address allocate(size_t size) {
      address addr;
      addr = current;
      current = (address)((uintptr_t)current + size);

      return addr;
    }

    void put_back(size_t size) {
      current = (address)((uintptr_t)current - size);
    }

    bool contains_p(address addr) {
      if(addr < start) return false;
      if(addr >= last) return false;
      return true;
    }

    bool enough_space_p(size_t size) {
      if((uintptr_t)current + size > (uintptr_t)last) return false;
      return true;
    }

    address try_allocate(size_t size) {
      address addr = current;
      address next = (address)((uintptr_t)current + size);
      if(next >= last) return 0;

      current = next;
      return addr;
    }

    bool fully_scanned_p() {
      return scan == current;
    }

    Object* next_unscanned(VM* state) {
      Object* obj;
      if(fully_scanned_p()) return NULL;

      obj = (Object*)scan;
      scan = (address)((uintptr_t)scan + obj->size_in_bytes(state));
      return obj;
    }

    Object* first_object() {
      return (Object*)start;
    }

    // Set the scan pointer to +addr+
    void set_scan(address addr) {
      scan = addr;
    }

    /* Prototypes */
    Heap(size_t size);
    ~Heap();
    void reset();
    size_t remaining();
    size_t used();
    Object* copy_object(VM* state, Object*);
  };

}

#endif
