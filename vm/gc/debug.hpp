#include "gc/gc.hpp"
#include "gc/root.hpp"

#include <map>

namespace rubinius {

  class Object;


  class HeapDebug : public GarbageCollector {
  public:
    int seen_objects;
    std::map <Object*, int> seen;

    HeapDebug(ObjectMemory *om);
    virtual ~HeapDebug() { }

    virtual Object* saw_object(Object* obj);
    virtual void scanned_object(Object* obj) {}
    virtual bool mature_gc_in_progress() { return false; }
    void walk(Roots &roots);
  };
}
