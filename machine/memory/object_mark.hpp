#ifndef RBX_GC_OBJECT_MARK_HPP
#define RBX_GC_OBJECT_MARK_HPP

namespace rubinius {

  class VM;
  class Object;

namespace memory {
  class GarbageCollector;

  /**
   * Used by GarbageCollector implementations to mark objects that are still
   * reachable. ObjectMark carries a GarbageCollector instance with it as it
   * visits an object and its contents.
   */

  class ObjectMark {
  public:
    GarbageCollector* gc;

  public:
    ObjectMark(GarbageCollector* gc)
      : gc(gc)
    {}

    VM* vm();
    bool mature_gc_in_progress();

    Object* call(Object*);
    void set(Object* target, Object** pos, Object* val);
    void just_set(Object* target, Object* val);
  };
}
}

#endif
