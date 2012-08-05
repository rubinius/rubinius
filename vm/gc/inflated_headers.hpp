#include <stddef.h>
#include <list>
#include "prelude.hpp"
#include "util/allocator.hpp"

namespace rubinius {
  class ObjectHeader;
  class InflatedHeader;
  class VM;


  /**
   * Manages a list of InflatedHeader instances.
   *
   * Storage for InflatedHeader objects are allocated in chunks, and these are
   * in turn stored in a linked list. As ObjectHeader instances are inflated,
   * they are added to the next free spot, or if no slots exist, a new chunk
   * is allocated.
   *
   * As InflatedHeader instances are deflated, they are added to free_list_,
   * which re-uses the storage of the InflatedHeader to link to the next free
   * InflatedHeader slot.
   */

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

    InflatedHeader* allocate(ObjectHeader* obj);
    void deallocate_headers(int mark);
    void allocate_chunk();
  };
}
