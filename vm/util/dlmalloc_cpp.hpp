#include "util/dlmalloc.h"

class DLMalloc {
  mspace space_;

public:
  DLMalloc(size_t capacity = 0, int locked = 0) {
    space_ = create_mspace(capacity, locked);
    assert(space_);
  }

  ~DLMalloc() {
    destroy_mspace(space_);
  }

  void* allocate(size_t bytes) {
    return mspace_malloc(space_, bytes);
  }

  void* reallocate(void* ptr, size_t bytes) {
    return mspace_realloc(space_, ptr, bytes);
  }

  void release(void* ptr) {
    mspace_free(space_, ptr);
  }

  size_t footprint() {
    return mspace_footprint(space_);
  }

  void print_stats() {
    mspace_malloc_stats(space_);
  }

  bool trim(size_t pad = 0) {
    return mspace_trim(space_, pad) == 1;
  }
};
