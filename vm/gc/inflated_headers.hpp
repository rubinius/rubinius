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
    VM* state_;
    Allocator<InflatedHeader>* allocator_;

  public:
    InflatedHeaders(VM* state)
      : state_(state)
      , allocator_(new Allocator<InflatedHeader>())
    {}

    ~InflatedHeaders() {
      delete allocator_;
    }

    InflatedHeader* allocate(ObjectHeader* obj, uint32_t* index);

    void deallocate_headers(int mark);
    void allocate_chunk();
  };
}
