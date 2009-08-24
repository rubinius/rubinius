#include <cstdlib>
#include "gc/heap.hpp"
#include "instruments/stats.hpp"

namespace rubinius {
  /* Heap methods */
  Heap::Heap(size_t bytes) {
    size = bytes;
    start = reinterpret_cast<address>(std::malloc(size));
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

  Object* Heap::copy_object(STATE, Object* orig) {
    size_t bytes = orig->size_in_bytes(state);
    Object* tmp = (Object*)allocate(bytes);
    tmp->init_header(YoungObjectZone, orig->type_id());

    tmp->initialize_copy(orig, orig->age);
    tmp->copy_body(state, orig);

#ifdef RBX_GC_STATS
    stats::GCStats::get()->objects_copied++;
    stats::GCStats::get()->bytes_copied += bytes;
#endif

    return tmp;
  }
}
