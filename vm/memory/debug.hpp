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

    virtual Object* saw_object(Object* obj);
    virtual void scanned_object(Object* obj) {}
    virtual bool mature_gc_in_progress() { return false; }
    void walk(Roots &roots);
  };
}
}
