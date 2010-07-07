#include <cstdlib>
#include "gc/heap.hpp"
#include "instruments/stats.hpp"

namespace rubinius {
  /* Heap methods */
  Heap::Heap(size_t bytes)
    : size_(bytes)
    , owner_(true)
  {
    start_ = reinterpret_cast<address>(std::malloc(size_));
    last_ = (void*)((uintptr_t)start_ + bytes - 1);

    int red_zone = bytes / 1024;
    limit_ = (address)((uintptr_t)last_ - red_zone);

    reset();
  }

  Heap::Heap(void* start, size_t bytes)
    : start_(start)
    , size_(bytes)
    , owner_(false)
  {
    last_ = (void*)((uintptr_t)start_ + bytes - 1);

    int red_zone = bytes / 1024;
    limit_ = (address)((uintptr_t)last_ - red_zone);

    reset();
  }

  Heap::~Heap() {
    if(owner_) {
      std::free(start_);
    }
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

    tmp->initialize_full_state(state, orig, orig->age());

#ifdef RBX_GC_STATS
    stats::GCStats::get()->objects_copied++;
    stats::GCStats::get()->bytes_copied += bytes;
#endif

    return tmp;
  }
}
