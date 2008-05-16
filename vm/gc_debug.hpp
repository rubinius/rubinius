#include "object.hpp"
#include "gc.hpp"
#include "gc_root.hpp"

#include <map>

namespace rubinius {
  class HeapDebug : public GarbageCollector {
  public:
    int seen_objects;
    std::map <OBJECT, int> seen;

    HeapDebug(ObjectMemory *om);
    virtual ~HeapDebug() { }

    virtual OBJECT saw_object(OBJECT obj);
    void walk(Roots &roots);
  };
}
