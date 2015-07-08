#ifndef RBX_VM_GC_BAKER_HPP
#define RBX_VM_GC_BAKER_HPP

#include <iostream>
#include <string.h>
#include <sys/mman.h>

#include "gc/heap.hpp"
#include "gc/gc.hpp"
#include "gc/root.hpp"
#include "object_position.hpp"
#include "diagnostics.hpp"

#include "builtin/object.hpp"

#include "object_watch.hpp"

#include "util/thread.hpp"

#ifdef HAVE_VALGRIND_H
#include <valgrind/memcheck.h>
#endif

namespace rubinius {

  class GCData;

  /**
   * The BakerGC garbage collector is used for managing memory in the young
   * generation.
   *
   * The BakerGC takes advantage of the fact that most newly created objects
   * will die young. As a result, most objects encountered when collecting the
   * young generation will be garbage; this makes it efficient to copy the
   * surviving objects to a new location, and then reclaim the memory from the
   * collected area in one step.
   *
   * Memory for the young generation is thus split into three spaces:
   * - the Eden space, where new objects are initially allocated;
   * - the Current space, where young objects that have survived at least one
   *   collection (but not enough for promotion to the mature generation) are
   *   located; and
   * - the Next space, where surviving objects from the Eden and Current spaces
   *   will be copied on the next collection.
   *
   * When a young generation collection is required, the garbage collector
   * scans the Eden and Current spaces. When it finds a live object, it either
   * copies it to the Next space, or promotes it to the mature generation,
   * depending on how many collections it has survived.
   *
   * On the rare occasions that there is insufficient space in the Next space to
   * hold all surviving young objects during a collection, then all remaining
   * live young objects are promoted directly to the mature generation.
   *
   * At completion, the allocation pointer is reset in both the Eden and
   * Current spaces, and then the Current and Next spaces are swapped.
   */

  class BakerGC : public GarbageCollector {
  public:
    class Diagnostics : public diagnostics::Diagnostics {
    public:
      const static int cPercentTiles = 10;

      int64_t collections_;
      int64_t occupancy_histo_[cPercentTiles];

      Diagnostics()
        : diagnostics::Diagnostics()
        , collections_(0)
      {
        for(int i = 0; i < cPercentTiles; i++) {
          occupancy_histo_[i] = 0;
        }
      }

      void record_occupancy(double percentage) {
        modify();
        collections_++;

        if(percentage < 0.0 || percentage > 100.0) return;

        occupancy_histo_[int(percentage / cPercentTiles)]++;
      }

      void log();
    };

  private:

    unsigned int bytes_;

    /// The total memory heap allocated to the BakerGC
    Heap* full;

    /// The Eden space heap, which is a subset of the full heap
    Heap* eden;

    /// The A heap, which is a subset of the full heap
    Heap* heap_a;

    /// The B heap, which is a subset of the full heap
    Heap* heap_b;


    /// Pointer to the Current space heap (i.e. Heap A or B)
    Heap* current;

    /// Pointer to the Next space heap (i.e. Heap B or A)
    Heap* next;

  public:
    /**
     * Allocates a slab of memory from the Eden space for use as a thread-local
     * allocation area that can be used without locking.
     */
    void* allocate_for_slab(size_t bytes) {
      if(!eden->enough_space_p(bytes)) {
        return NULL;
      }

      void* addr = 0;

      addr = eden->allocate(bytes);

      // If this takes us over the limit, return the bytes we just grabbed
      if(eden->over_limit_p(addr)) {
          eden->put_back(bytes);
          return NULL;
      }

      return addr;
    }

    /* Inline methods */

    /**
     * Attempts to allocate an object of the specified size from the Eden heap.
     * Unlike allocate, the header of the returned object is not initialized.
     *
     * If there is insufficient space remaining, NULL is returned and the
     * limit_hit parameter is set to true.
     */
    Object* raw_allocate(size_t bytes, bool* limit_hit) {
      Object* obj;

      if(!eden->enough_space_p(bytes)) {
        return NULL;
      } else {
        obj = eden->allocate(bytes).as<Object>();

        if(eden->over_limit_p(obj)) {
          *limit_hit = true;
        }
      }

#ifdef ENABLE_OBJECT_WATCH
      if(watched_p(obj)) {
        std::cout << "detected " << obj << " during baker allocation.\n";
      }
#endif

      return obj;
    }


    /**
     * Attempts to allocate an object of the specified size from the Eden heap.
     *
     * If successful, the returned object's header is initiliazed to the young
     * generation.
     *
     * If there is insufficient space remaining, NULL is returned and the
     * limit_hit parameter is set to true.
     */
    Object* allocate(size_t bytes, bool* limit_hit) {
      Object* obj;

      if(!eden->enough_space_p(bytes)) {
        return NULL;
      } else {
        obj = eden->allocate(bytes).as<Object>();

        if(eden->over_limit_p(obj)) {
          *limit_hit = true;
        }
      }

#ifdef ENABLE_OBJECT_WATCH
      if(watched_p(obj)) {
        std::cout << "detected " << obj << " during baker allocation.\n";
      }
#endif

      obj->init_header(YoungObjectZone, InvalidType);

      return obj;
    }

  private:

    /// List of objects that have been promoted to the mature generation, but
    /// not yet scanned
    ObjectArray promoted_stack_;

    /// The current lifetime objects have to survive before being promoted
    unsigned int lifetime_;

    Diagnostics diagnostics_;

    /**
     * Adds the specified object to the promoted objects stack.
     * Objects on the promoted_objects_ stack must be scanned after the
     * remaining young objects have been copied, since only then can we know
     * which references from the promoted object need to be added to the
     * remember set.
     */
    void promoted_push(Object* obj) {
      promoted_stack_.push_back(obj);
    }

  public:

    /**
     * Returns the number of bytes used in the Eden space.
     */
    size_t bytes_used() {
      return eden->used();
    }

    /// Returns the start address of the young generation
    void* start_address() {
      return full->start();
    }

    /// Returns the last address in the young generation
    void* last_address() {
      return full->last();
    }

    void reset();

    Diagnostics& diagnostics() {
      return diagnostics_;
    }

  public:
    /* Prototypes */
    BakerGC(ObjectMemory *om, Configuration& config);
    virtual ~BakerGC();

    /// Called for each object "seen" during the collection of the young
    /// generation.
    virtual Object* saw_object(Object* obj);

    /// Called for each object after the object is scanned
    virtual void scanned_object(Object* obj) {}

    /// Called whether there's a mature mark phase in progress
    virtual bool mature_gc_in_progress() { return false; }

    /// Scans the remaining unscanned portion of what was the Current space.
    void    copy_unscanned();

    /// Returns true if the young generation has been fully scanned in the
    /// current collection.
    bool    fully_scanned_p();

    /// Performs a collection of the young generation.
    void    collect(GCData* data);

    /// Gathers statistics on the age of objects at the time of their death.
    void    find_lost_souls();

    /// Scans the finalizers for young generation objects that need to be kept
    /// alive.
    void    walk_finalizers();

    /// Scans any references held by objects that have been promoted to the
    /// mature generation.
    bool    handle_promotions();

    /// Scans the pending mark set
    void    scan_mark_set();

    /// Updates the pending mark set and removes any young unreachable objects
    void    update_mark_set();

    /// Scans the current mark stack
    void    scan_mature_mark_stack();

    /// Updates the current mark stack and removes any young unreachable objects
    void    update_mature_mark_stack();

    /// Updates the set with weak refs and removes any young unreachable objects.
    /// Note this removes the actual WeakRef, not the object referenced.
    void    update_weak_refs_set();

    /// Validates the specified object, and returns an ObjectPosition value
    /// indicating in which space in the young generation the object lies.
    ObjectPosition validate_object(Object* obj);

    /// Returns true if the specified object is in the Current space.
    bool in_current_p(Object* obj);

    double percentage_used() {
      return current->percentage_used();
    }
  };
};

#endif
