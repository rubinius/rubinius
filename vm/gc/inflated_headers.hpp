#include <stddef.h>
#include <list>

namespace rubinius {
  class ObjectHeader;
  class InflatedHeader;

  class InflatedHeaders {
  public:
    typedef std::list<InflatedHeader*> Chunks;
    static const size_t cChunkSize = 1024;

  private:
    Chunks chunks_;
    InflatedHeader* free_list_;
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
