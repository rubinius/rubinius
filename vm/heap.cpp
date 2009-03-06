#include <cstdlib>
#include "vm/heap.hpp"
#include "instruments/stats.hpp"

namespace rubinius {
  /* Heap methods */
  Heap::Heap(size_t bytes) {
    size = bytes;
    start = (address)std::calloc(1, size);
    scan = start;
    last = (void*)((uintptr_t)start + bytes - 1);
    reset();
  }

  Heap::~Heap() {
    std::free(start);
  }

  void Heap::reset() {
    current = start;
    scan = start;
  }

  size_t Heap::remaining() {
    size_t bytes = (uintptr_t)last - (uintptr_t)current;
    return bytes;
  }

  size_t Heap::used() {
    size_t bytes = (uintptr_t)current - (uintptr_t)start;
    return bytes;
  }

  Object* Heap::copy_object(Object* orig) {
    assert(orig->size_in_bytes() % 4 == 0);
    Object* tmp = (Object*)allocate(orig->size_in_bytes());
    tmp->init_header(YoungObjectZone, orig->num_fields());

    tmp->initialize_copy(orig, orig->age);
    tmp->copy_body(orig);

#ifdef RBX_GC_STATS
    stats::GCStats::get()->objects_copied++;
    stats::GCStats::get()->bytes_copied += orig->size_in_bytes();
#endif

    return tmp;
  }

}
