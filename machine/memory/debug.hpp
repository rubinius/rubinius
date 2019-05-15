#include "memory/gc.hpp"
#include "memory/root.hpp"

#include <map>

namespace rubinius {
  class Object;

namespace memory {

  class HeapDebug : public GarbageCollector {
  public:
    int seen_objects;
    std::map <Object*, int> seen;

    HeapDebug(Memory *om);
    virtual ~HeapDebug() { }

    virtual Object* saw_object(void* parent, Object* child);
    virtual void scanned_object(Object* obj) {}
    void walk(Roots &roots);
  };
}
}
