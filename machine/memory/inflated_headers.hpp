#ifndef RBX_GC_INFLATED_HEADERS_HPP
#define RBX_GC_INFLATED_HEADERS_HPP

#include "defines.hpp"
#include "memory/allocator.hpp"

#include "diagnostics.hpp"

#include <stddef.h>
#include <list>

namespace rubinius {
  class ObjectHeader;
  class InflatedHeader;
  class VM;

namespace memory {

  class InflatedHeaders {
  public:

  private:
    Allocator<InflatedHeader>* allocator_;

    diagnostics::InflatedHeader* diagnostic_;

  public:
    InflatedHeaders();
    ~InflatedHeaders();

    InflatedHeader* allocate(STATE, ObjectHeader* obj, uint32_t* index);

    InflatedHeader* find_index(uintptr_t index) {
      return allocator_->from_index(index);
    }

    void deallocate_headers(unsigned int mark);

    int size() const {
      return allocator_->in_use_;
    }

    diagnostics::InflatedHeader* diagnostic() {
      return diagnostic_;
    }
  };
}
}

#endif
