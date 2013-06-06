#ifndef RBX_VM_GC_BAKER_HPP
#define RBX_VM_GC_BAKER_HPP

#include <iostream>
#include <string.h>
#include <sys/mman.h>

#include "gc/heap.hpp"
#include "gc/gc.hpp"
#include "gc/root.hpp"
#include "object_position.hpp"

#include "builtin/object.hpp"

#include "object_watch.hpp"

#include "util/thread.hpp"

#ifdef HAVE_VALGRIND_H
#include <valgrind/memcheck.h>
#endif

namespace rubinius {

  class GCData;
  struct YoungCollectStats;

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
   *   wil be copied on the next collection.
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

    /// The total memory heap allocated to the BakerGC
    Heap* full;

    /// The Eden space heap, which is a subset of the full heap
    Heap* eden;

    /// The A heap, which is a subset of the full heap
    Heap* heap_a;

    /// The B heap, which is a subset of the full heap
    Heap* heap_b;


    /// The new heap if we request a resize. We need to set
    /// this up at the start of a young GC cycle and clean
    /// up at the end
    Heap* full_new;

    /// The new Eden space heap when resizing
    Heap* eden_new;

    /// The new A heap when resizing
    Heap* heap_a_new;

    /// The new B heap when resizing
    Heap* heap_b_new;

    /// Pointer to the Current space heap (i.e. Heap A or B)
    Heap* current;

    /// Pointer to the Next space heap (i.e. Heap B or A)
    Heap* next;

  public:
    /// Total number of objects currently allocated in the young generation
    /// Note: This does not include objects allocated in the Eden space in
    /// thread-local slabs.
    size_t total_objects;

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
        total_objects++;
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
        total_objects++;
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

    /// Current size of the young gen, number of bytes in Eden
    size_t current_byte_size_;

    /// Request size of new young space for resizing
    size_t requested_byte_size_;

    /// The original configured lifetime objects had to survive before being
    /// promoted to the mature generation
    unsigned int original_lifetime_;

    /// The current lifetime objects have to survive before being promoted
    unsigned int lifetime_;

    /// Count of the number of objects that were promoted to the mature
    /// generation on the current collection due to lack of space in the Next
    /// space.
    int copy_spills_;

    /// Count of the number of objects promoted on the current collection
    int promoted_objects_;

    /// Count of the successive number of times a collection has gone over
    /// the cOverFullThreshhold (positive value) or under the
    /// cUnderFullThreshhold (negative value). Used to auto-tune the age
    /// at which objects are promoted to the mature generation.
    int tune_threshold_;

    /// True if the young to mature promotion threshhold should be auto-tuned
    bool autotune_lifetime_;

    /**
     * Adds the specified object to the promoted objects stack.
     * Objects on the promoted_objects_ stack must be scanned after the
     * remaining young objects have been copied, since only then can we know
     * which references from the promoted object need to be added to the
     * remember set.
     */
    void promoted_push(Object* obj) {
      promoted_objects_++;
      promoted_stack_.push_back(obj);
    }

    /**
     * Reset the list of promoted objects.
     */
    void reset_promoted() {
      promoted_objects_ = 0;
    }

    void check_growth_start();
    void check_growth_finish();

  public:

    /**
     * Returns the number of bytes used in the Eden space.
     */
    size_t bytes_used() {
      return eden->used();
    }

    /**
     * Returns the number of bytes used in the Eden space.
     */
    size_t& bytes_size() {
      return current_byte_size_;
    }

    /**
     * Sets the age at which objects will be promoted from the young generation
     * to the mature generation.
     */
    void set_lifetime(int val) {
      lifetime_ = original_lifetime_ = val;
    }

    /**
     * Controls whether the BakerGC is self-tuning.
     *
     * This involves tweaking the lifetime threshhold at which objects are
     * promoted to the mature generation, based on how full the Next space is
     * following a collecion.
     */
    void set_autotune_lifetime(bool val = true) {
      autotune_lifetime_ = val;
    }

    /// Returns true if the BakerGC is auto-tuning
    bool autotune_lifetime() {
      return autotune_lifetime_;
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

    void grow(size_t bytes) {
      requested_byte_size_ = bytes;
    }

  public:
    /* Prototypes */
    BakerGC(ObjectMemory *om, size_t size);
    virtual ~BakerGC();

    /// Called for each object "seen" during the collection of the young
    /// generation.
    virtual Object* saw_object(Object* obj);

    /// Called for each object after the object is scanned
    virtual void scanned_object(Object* obj) {}

    /// Scans the remaining unscanned portion of what was the Current space.
    void    copy_unscanned();

    /// Returns true if the young generation has been fully scanned in the
    /// current collection.
    bool    fully_scanned_p();

    /// Performs a collection of the young generation.
    void    collect(GCData* data, YoungCollectStats* stats = 0);

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

  };

};

#endif
