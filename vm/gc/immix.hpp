#ifndef RBX_GC_IMMIX
#define RBX_GC_IMMIX

#include "util/immix.hpp"
#include "gc.hpp"

namespace rubinius {
  class ObjectMemory;
  class ImmixGC;

  class ImmixGC : public GarbageCollector {
    class ObjectDescriber {
      ObjectMemory* object_memory_;
      ImmixGC* gc_;

    public:
      ObjectDescriber()
        : object_memory_(0)
      {}

      void set_object_memory(ObjectMemory* om, ImmixGC* gc) {
        object_memory_ = om;
        gc_ = gc;
      }

      void added_chunk(int count);

      void set_forwarding_pointer(immix::Address from, immix::Address to);

      immix::Address forwarding_pointer(immix::Address cur) {
        Object* obj = cur.as<Object>();

        if(obj->forwarded_p()) return obj->forward();

        return immix::Address::null();
      }

      immix::Address copy(immix::Address original, immix::Allocator& alloc) {
        Object* orig = original.as<Object>();

        immix::Address copy_addr = alloc.allocate(orig->size_in_bytes());
        Object* copy = copy_addr.as<Object>();

        copy->initialize_copy(orig, 0);
        copy->copy_body(orig);

        copy->zone = MatureObjectZone;
        copy->InImmix = 1;

        return copy_addr;
      }

      void walk_pointers(immix::Address addr, immix::Marker<ObjectDescriber>& mark) {
        gc_->scan_object(addr.as<Object>());
      }

      int size(immix::Address addr) {
        return addr.as<Object>()->size_in_bytes();
      }

      bool mark_address(immix::Address addr, immix::MarkStack& ms) {
        Object* obj = addr.as<Object>();

        if(obj->marked_p()) return false;
        obj->mark();

        ms.push_back(addr);

        // If this is a young object, let the GC know not to try and mark
        // the block it's in.
        if(obj->young_object_p() || !obj->InImmix) {
          return false;
        }
        return true;
      }
    };

    immix::GC<ObjectDescriber> gc_;
    immix::ExpandingAllocator allocator_;

  public:
    ImmixGC(ObjectMemory* om);
    virtual ~ImmixGC();

    Object* allocate(int bytes);

    virtual Object* saw_object(Object*);
    void collect(GCData& data);
  };
}

#endif
