#include <stddef.h>
#include <list>

namespace rubinius {
  class ObjectHeader;
  class InflatedHeader;


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
  public:
    typedef std::list<InflatedHeader*> Chunks;

    /// Storage for InflatedHeader references is allocated in chunks.
    static const size_t cChunkSize = 1024;

  private:
    /// Linked list of InflatedHeader pointers.
    Chunks chunks_;

    /// Pointer to the first free InflatedHeader slot in the list.
    InflatedHeader* free_list_;

    /// Number of in-use slots
    int in_use_;

  public:
    InflatedHeaders()
      : free_list_(0)
      , in_use_(0)
    {}

    ~InflatedHeaders();

    InflatedHeader* allocate(ObjectHeader* obj);
    void deallocate_headers(int mark);
    void allocate_chunk();
  };
}
