#ifndef RBX_GC_IMMIX
#define RBX_GC_IMMIX

#include "util/immix.hpp"
#include "gc/gc.hpp"
#include "exception.hpp"

#include "object_position.hpp"

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
      void last_block();

      void set_forwarding_pointer(immix::Address from, immix::Address to);

      immix::Address forwarding_pointer(immix::Address cur) {
        Object* obj = cur.as<Object>();

        if(obj->forwarded_p()) return obj->forward();

        return immix::Address::null();
      }

      bool pinned(immix::Address addr) {
        return addr.as<Object>()->pinned_p();
      }

      immix::Address copy(immix::Address original, immix::Allocator& alloc);

      void walk_pointers(immix::Address addr, immix::Marker<ObjectDescriber>& mark) {
        gc_->scan_object(addr.as<Object>());
      }

      int size(immix::Address addr);

      bool mark_address(immix::Address addr, immix::MarkStack& ms) {
        Object* obj = addr.as<Object>();

        if(obj->marked_p(object_memory_->mark())) return false;
        obj->mark(object_memory_->mark());
        gc_->inc_marked_objects();

        ms.push_back(addr);
        if(obj->in_immix_p()) return true;

        // If this is a young object, let the GC know not to try and mark
        // the block it's in.
        return false;
      }
    };

    immix::GC<ObjectDescriber> gc_;
    immix::ExpandingAllocator allocator_;
    int marked_objects_;
    int chunks_left_;
    int chunks_before_collection_;

  public:
    ImmixGC(ObjectMemory* om);
    virtual ~ImmixGC();

    Object* allocate(int bytes);

    virtual Object* saw_object(Object*);
    void collect(GCData& data);

    void check_finalize();

    ObjectPosition validate_object(Object*);

  public: // Inline
    size_t& bytes_allocated() {
      return gc_.bytes_allocated();
    }

    void inc_marked_objects() {
      marked_objects_++;
    }

    int marked_objects() {
      return marked_objects_;
    }

    int clear_marked_objects() {
      int m = marked_objects_;
      marked_objects_ = 0;
      return m;
    }

    int dec_chunks_left() {
      return --chunks_left_;
    }

    void reset_chunks_left() {
      chunks_left_ = chunks_before_collection_;
    }
  };
}

#endif
