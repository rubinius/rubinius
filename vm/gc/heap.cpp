#include <cstdlib>
#include "gc/heap.hpp"
#include "instruments/stats.hpp"

namespace rubinius {
  /* Heap methods */
  Heap::Heap(size_t bytes) {
    size_ = bytes;
    start_ = reinterpret_cast<address>(std::malloc(size_));
    scan_ = start_;
    last_ = (void*)((uintptr_t)start_ + bytes - 1);

    int red_zone = bytes / 1000;
    limit_ = (address)((uintptr_t)last_ - red_zone);

    reset();
  }

  Heap::~Heap() {
    std::free(start_);
  }

  void Heap::reset() {
    current_ = start_;
    scan_ = start_;
  }

  size_t Heap::remaining() {
    size_t bytes = (uintptr_t)last_ - (uintptr_t)current_;
    return bytes;
  }

  size_t Heap::used() {
    size_t bytes = (uintptr_t)current_ - (uintptr_t)start_;
    return bytes;
  }

  Object* Heap::copy_object(STATE, Object* orig) {
    size_t bytes = orig->size_in_bytes(state);
    Object* tmp = (Object*)allocate(bytes);
    tmp->init_header(YoungObjectZone, orig->type_id());

    tmp->initialize_copy(orig, orig->age());
    tmp->copy_body(state, orig);

#ifdef RBX_GC_STATS
    stats::GCStats::get()->objects_copied++;
    stats::GCStats::get()->bytes_copied += bytes;
#endif

    return tmp;
  }
}
