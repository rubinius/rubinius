#ifndef RBX_GC_WRITE_BARRIER_HPP
#define RBX_GC_WRITE_BARRIER_HPP

#include "util/thread.hpp"

namespace rubinius {

  class Object;

namespace gc {

  /**
   * A write barrier keeps track of objects in the mature generation that hold
   * references to objects in the young generation.
   *
   * A write barrier allows the young generation to be garbage collected without
   * inspecting every object in the older generations (mature and large object)
   * to determine if a young object is still referenced (i.e. alive).  As most
   * cross-generation references are from young objects to mature ones,
   * remembering the rarer instances when a mature object has a reference to a
   * young object results in a smaller set of objects to scan when collecting
   * the young generation.
   *
   * The set of locations containing pointers to young generation objects is
   * known as the remembered set. At every store, the system must ensure that
   * the updated location is added to the remembered set if the store creates
   * a reference from an older generation object to a young generation object.
   *
   * The remembered set is cleared on each collection of the young generation
   * (via #swap_remember_set), as scanning objects that were in the remember set
   * as part of the young collection will re-remember mature objects that
   * continue to hold a reference to (still) young objects.
   *
   * Additionally, when a full collection is performed, any mature objects
   * that die are removed from the remember set (via #unremember_object and
   * #unremember_objects), ensuring they do not keep alive young objects they
   * reference.
   */

  class WriteBarrier {
    /// Lock for synchronising multi-threaded updates to the remember set
    utilities::thread::SpinLock lock_;

  protected:

    /// Holds a list of remembered Object instances in the mature generation
    /// that contain one or more references to young generation objects.
    ObjectArray* remember_set_;

    /// Holds a list of rescan Object instances in the mature generation
    /// that contain one or more references to unmarked objects.
    ObjectArray* marked_set_;
  public:

    /**
     * Returns the current remembered set of mature objects.
     * The returned ObjectArray* is const, since it should not be modified other
     * than via the remember/unremember methods.
     */
    ObjectArray* const remember_set() {
      return remember_set_;
    }

    /**
     * Returns the current mark set
     */
    ObjectArray* marked_set() {
      return marked_set_;
    }

    WriteBarrier();
    ~WriteBarrier();

    /**
     * Checks if the store is creating a reference from a mature generation
     * object (target) to a young generation object (val). If it is, target
     * (i.e. the mature object) is added to the remember set.
     *
     * @param target The object holding the reference (i.e. the referer).
     * @param val    The object being referenced (i.e. the referee).
     */
    void inline write_barrier(ObjectHeader* target, ObjectHeader* val, int mark) {
      if(!val->reference_p()) return;
      if(target->reference_p()) {
        // Check if we need to mark the value set because the
        // target is already scanned.
        if(unlikely(target->scanned_p(mark) && !val->marked_p(mark))) {
          mark_object(reinterpret_cast<Object*>(val));
        }
        if(target->remembered_p()) return;
        if(target->young_object_p()) return;
        if(!val->young_object_p()) return;
      }

      remember_object(reinterpret_cast<Object*>(target));
    }

    // Adds the target object directly to the remembered set.
    void remember_object(Object* target);

    // Removes the mature Object +target+ from the remembered set.
    void unremember_object(Object* target);
    // Removes objects from the remember set that do not have the specified mark.
    int unremember_objects(unsigned int mark);

    // Adds the target object directly to the remembered set.
    void mark_object(Object* target);

    // Returns the current remember set, and replaces it with a new, empty one.
    ObjectArray* swap_remember_set();

    // Returns the current marked set, and replaces it with a new, empty one.
    ObjectArray* swap_marked_set();
  };

}}

#endif
