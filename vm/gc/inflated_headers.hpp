#include <stddef.h>
#include <list>
#include "prelude.hpp"
#include "util/allocator.hpp"

namespace rubinius {
  class ObjectHeader;
  class InflatedHeader;
  class VM;

  class InflatedHeaders {
  private:
    Allocator<InflatedHeader>* allocator_;

  public:
    InflatedHeaders()
      : allocator_(new Allocator<InflatedHeader>())
    {}

    ~InflatedHeaders() {
      delete allocator_;
    }

    InflatedHeader* allocate(STATE, ObjectHeader* obj, uint32_t* index);

    InflatedHeader* find_index(uintptr_t index) {
      return allocator_->from_index(index);
    }

    void deallocate_headers(unsigned int mark);
    void allocate_chunk();

    int size() const {
      return allocator_->in_use_;
    }
  };
}
