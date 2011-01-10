#ifndef RBX_GC_WRITE_BARRIER_HPP
#define RBX_GC_WRITE_BARRIER_HPP

#include "util/thread.hpp"

namespace rubinius {
namespace gc {

  /**
   * A write barrier keeps track of references from older generations to the
   * young generation.
   *
   * A write barrier allows the young generation to be garbage collected without
   * inspecting every object in the older generations (mature and large object)
   * to determine if a young object is still referenced (i.e. alive).
   * As most cross-generation references are from young objects to mature ones,
   * remembering the rarer instances when a mature object has a reference to a
   * young object results in a smaller set of objects to scan when collecting
   * the young generation.
   *
   * The set of locations containing pointers to young generation objects is
   * known as the remembered set. At every store, the system must ensure that
   * the updated location is added to the remembered set if the store creates
   * a reference from an older generation object to a young generation object.
   *
   * The remembered set is cleared on each collection of the young generation,
   * as scanning objects during the collection will re-remember objects that
   * continue to hold a reference to young generation objects.
   */

  class WriteBarrier {
    thread::SpinLock lock_;

  protected:

    /// Holds a list of remembered Object instances in the mature generation
    /// that contain one or more references to young generation objects.
    ObjectArray* remember_set_;

  public:

    /**
     * Returns the current remembered set of mature objects.
     */
    ObjectArray* remember_set() {
      return remember_set_;
    }

    WriteBarrier();
    ~WriteBarrier();

    /**
     * Checks if the store is creating a reference from a mature generation
     * object (target) to a young generation object (val). If it is, the mature
     * object is added to the remember set.
     *
     * \param target The object holding the reference (i.e. the referer).
     * \param val    The object being referenced (i.e. the referee).
     */
    void write_barrier(Object* target, Object* val) {
      if(target->remembered_p()) return;
      if(!REFERENCE_P(val)) return;
      if(target->zone() == YoungObjectZone) return;
      if(val->zone() != YoungObjectZone) return;

      remember_object(target);
    }

    /// Adds the target object directly to the remembered set.
    void remember_object(Object* target);

    /// Removes the target object from the remembered set.
    void unremember_object(Object* target);

    /// Returns the current remember set, and replaces it with a new, empty one.
    ObjectArray* swap_remember_set();
  };

}}

#endif
