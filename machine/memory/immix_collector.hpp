#ifndef RBX_GC_IMMIX_HPP
#define RBX_GC_IMMIX_HPP

#include "memory/address.hpp"
#include "memory/immix_region.hpp"
#include "memory/gc.hpp"

#include "exception.hpp"
#include "object_position.hpp"

#include "diagnostics.hpp"

namespace rubinius {
  class Memory;

namespace memory {
  class ImmixGC;

  /**
   * ImmixGC uses the immix memory management strategy to perform garbage
   * collection on the mature objects in the immix space.
   */

  class ImmixGC : public GarbageCollector {
    /**
     * Class used as an interface to the Rubinius specific object memory layout
     * by the (general purpose) Immix memory manager. By imposing this interface
     * between Memory and the utility Immix memory manager, the latter can
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
      Memory* memory_;
      ImmixGC* gc_;

    public:
      ObjectDescriber()
        : memory_(0)
        , gc_(NULL)
      {}

      void set_object_memory(Memory* om, ImmixGC* gc) {
        memory_ = om;
        gc_ = gc;
      }

      void added_chunk(int count);
      void last_block() { }

      void set_forwarding_pointer(Address from, Address to);

      Address forwarding_pointer(Address cur) {
        Object* obj = cur.as<Object>();

        if(obj->forwarded_p()) return obj->forward();

        return Address::null();
      }

      bool pinned(Address addr) {
        return addr.as<Object>()->pinned_p();
      }

      Address copy(Address original, ImmixAllocator& alloc);

      void walk_pointers(Address addr, Marker<ObjectDescriber>& mark) {
        Object* obj = addr.as<Object>();
        if(obj) gc_->scan_object(obj);
      }

      Address update_pointer(Address addr);

      int size(Address addr);

      /**
       * Called when the GC object wishes to mark an object.
       *
       * @returns true if the object is not already marked, and in the Immix
       * space; otherwise false.
       */
      bool mark_address(Address addr, MarkStack& ms, bool push = true);
    };

    GC<ObjectDescriber> gc_;
    ExpandingAllocator allocator_;
    Memory* memory_;
    int chunks_left_;
    int chunks_before_collection_;
    diagnostics::Immix* diagnostic_;

  public:
    ImmixGC(Memory* om);
    virtual ~ImmixGC();

    Object* allocate(size_t bytes, bool& collect_now);
    Object* move_object(Object* orig, size_t bytes, bool& collect_now);

    virtual Object* saw_object(Object*);
    virtual void scanned_object(Object*);
    virtual bool mature_gc_in_progress();
    void collect(GCData* data);
    void collect_start(GCData* data);
    void collect_finish(GCData* data);
    void sweep(GCData* data);

    void walk_finalizers();

    ObjectPosition validate_object(Object*);

  public: // Inline
    Memory* memory() {
      return memory_;
    }

    size_t& bytes_allocated() {
      return gc_.bytes_allocated();
    }

    int dec_chunks_left() {
      return --chunks_left_;
    }

    void reset_chunks_left() {
      chunks_left_ = chunks_before_collection_;
    }

    diagnostics::Immix* diagnostic() {
      return diagnostic_;
    }

    bool process_mark_stack();
    bool process_mark_stack(bool& exit);
    MarkStack& mark_stack();

  private:
    void collect_scan(GCData* data);
  };
}
}

#endif
