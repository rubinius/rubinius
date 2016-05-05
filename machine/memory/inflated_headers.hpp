#ifndef RBX_GC_INFLATED_HEADERS_HPP
#define RBX_GC_INFLATED_HEADERS_HPP

#include "diagnostics.hpp"
#include "defines.hpp"
#include "memory/allocator.hpp"

#include <stddef.h>
#include <list>

namespace rubinius {
  class ObjectHeader;
  class InflatedHeader;
  class VM;

namespace memory {

  class InflatedHeaders {
  public:
    class Diagnostics : public diagnostics::MemoryDiagnostics {
    public:
      int64_t collections_;

      Diagnostics()
        : diagnostics::MemoryDiagnostics()
        , collections_(0)
      { }

      void update();
    };

  private:
    Allocator<InflatedHeader>* allocator_;

    Diagnostics* diagnostics_;

  public:
    InflatedHeaders()
      : allocator_(new Allocator<InflatedHeader>())
      , diagnostics_(new Diagnostics())
    {}

    ~InflatedHeaders() {
      delete allocator_;
      if(diagnostics()) delete diagnostics();
    }

    InflatedHeader* allocate(STATE, ObjectHeader* obj, uint32_t* index);

    InflatedHeader* find_index(uintptr_t index) {
      return allocator_->from_index(index);
    }

    void deallocate_headers(unsigned int mark);

    int size() const {
      return allocator_->in_use_;
    }

    Diagnostics* diagnostics() {
      return diagnostics_;
    }
  };
}
}

#endif
