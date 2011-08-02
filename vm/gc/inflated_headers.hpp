#include <stddef.h>
#include <list>
#include "prelude.hpp"

namespace rubinius {
  class ObjectHeader;
  class InflatedHeader;
  class VM;

  class InflatedHeaders {
  public:
    typedef std::list<InflatedHeader*> Chunks;
    static const size_t cChunkSize  = 1024;
    static const size_t cChunkLimit = 16;

  private:
    VM* state_;
    Chunks chunks_;
    InflatedHeader* free_list_;
    size_t allocations_;
    int in_use_;

  public:
    InflatedHeaders(STATE)
      : state_(state)
      , free_list_(0)
      , allocations_(0)
      , in_use_(0)
    {}

    ~InflatedHeaders();

    InflatedHeader* allocate(ObjectHeader* obj);
    void deallocate_headers(int mark);
    void allocate_chunk();
  };
}
