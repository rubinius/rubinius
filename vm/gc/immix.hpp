#ifndef RBX_GC_IMMIX
#define RBX_GC_IMMIX

#include "util/address.hpp"
#include "util/immix.hpp"
#include "gc/gc.hpp"
#include "exception.hpp"

#include "object_position.hpp"

namespace rubinius {
  class ObjectMemory;
  class ImmixGC;
  class ImmixMarker;

  /**
   * ImmixGC uses the immix memory management strategy to perform garbage
   * collection on the mature objects in the immix space.
   */

  class ImmixGC : public GarbageCollector {

    /**
     * Class used as an interface to the Rubinius specific object memory layout
     * by the (general purpose) Immix memory manager. By imposing this interface
     * between ObjectMemory and the utility Immix memory manager, the latter can
     * be made reusable.
     *
     * The Immix memory manager delegates to this class to:
     * - determine the size of an object at a particular memory address
     * - copy an object
     * - return the forwarding pointer for an object
     * - set the forwarding pointer for an object that it moves
     * - mark an address as visited
     * - determine if an object is pinned and cannot be moved
     * - walk all root pointers
     *
     * It will also notify this class when:
     * - it adds chunks
     * - allocates from the last free block, indicating a collection is needed
     */
    class ObjectDescriber {
      ObjectMemory* object_memory_;
      ImmixGC* gc_;

    public:
      ObjectDescriber()
        : object_memory_(0)
        , gc_(NULL)
      {}

      void set_object_memory(ObjectMemory* om, ImmixGC* gc) {
        object_memory_ = om;
        gc_ = gc;
      }

      void added_chunk(int count);
      void last_block();

      void set_forwarding_pointer(memory::Address from, memory::Address to);

      memory::Address forwarding_pointer(memory::Address cur) {
        Object* obj = cur.as<Object>();

        if(obj->forwarded_p()) return obj->forward();

        return memory::Address::null();
      }

      bool pinned(memory::Address addr) {
        return addr.as<Object>()->pinned_p();
      }

      memory::Address copy(memory::Address original, immix::Allocator& alloc);

      void walk_pointers(memory::Address addr, immix::Marker<ObjectDescriber>& mark) {
        Object* obj = addr.as<Object>();
        if(obj) gc_->scan_object(obj);
      }

      memory::Address update_pointer(memory::Address addr) {
        Object* obj = addr.as<Object>();
        if(!obj) return memory::Address::null();
        if(obj->young_object_p()) {
          if(obj->forwarded_p()) return obj->forward();
          return memory::Address::null();
        } else {
          // we must remember this because it might
          // contain references to young gen objects
          object_memory_->remember_object(obj);
        }
        return addr;
      }

      int size(memory::Address addr);

      /**
       * Called when the immix::GC object wishes to mark an object.
       *
       * @returns true if the object is not already marked, and in the Immix
       * space; otherwise false.
       */
      inline bool mark_address(memory::Address addr, immix::MarkStack& ms, bool push = true) {
        Object* obj = addr.as<Object>();

        if(obj->marked_p(object_memory_->mark())) return false;
        obj->mark(object_memory_, object_memory_->mark());
        gc_->inc_marked_objects();

        if(push) ms.push_back(addr);
        // If this is a young object, let the GC know not to try and mark
        // the block it's in.
        return obj->in_immix_p();
      }
    };

    immix::GC<ObjectDescriber> gc_;
    immix::ExpandingAllocator allocator_;
    ImmixMarker* marker_;
    int marked_objects_;
    int chunks_left_;
    int chunks_before_collection_;

  public:
    ImmixGC(ObjectMemory* om);
    virtual ~ImmixGC();

    Object* allocate(uint32_t bytes);
    Object* move_object(Object* orig, uint32_t bytes);

    virtual Object* saw_object(Object*);
    virtual void scanned_object(Object*);
    void collect(GCData* data);
    void collect_start(GCData* data);
    void collect_finish(GCData* data);
    void sweep();
    void wait_for_marker(STATE);

    void walk_finalizers();

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

    void start_marker(STATE);
    bool process_mark_stack(int count = 0);
    immix::MarkStack& mark_stack();

  private:
    void collect_scan(GCData* data);
  };
}

#endif
