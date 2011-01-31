#include "builtin/object.hpp"
#include "gc/write_barrier.hpp"

namespace rubinius {
namespace gc {

  /**
   * Creates a remember set for the new WriteBarrier.
   */
  WriteBarrier::WriteBarrier()
    : remember_set_(new ObjectArray(0))
  {}

  /**
   * Deletes the remember set associated with this WriteBarrier.
   */
  WriteBarrier::~WriteBarrier() {
    delete remember_set_;
  }

  /**
   * Store the mature Object +target+ into the remember set.
   * Called when it is determined (elsewhere) that the +target+ object holds
   * one or more references to objects currently in the young generation.
   *
   * /param target The mature object to be added to the remember set.
   */
  void WriteBarrier::remember_object(Object* target) {
    thread::SpinLock::LockGuard lg(lock_);

    assert(target->zone() == MatureObjectZone);

    // If it's already remembered, ignore this request
    if(target->remembered_p()) return;
    target->set_remember();
    remember_set_->push_back(target);
  }

  /**
   * Called during collection of the mature generation to remove the mature
   * object +target+ from the remember set (if present).
   * When a mature collection is performed, any mature object that is collected
   * needs to be removed from the remember set, to ensure it does not keep
   * alive young generation objects that are no longer reachable.
   *
   * Note: This method performs a linear scan for the remember object in the
   * remember set, so is not efficient for large numbers of removals. The
   * ImmixGC thus clears the remember set directly in a single pass.
   * @todo Should the MarkSweepGC use the same method?
   *
   * /param target The mature object being collected (deleted), and therefore
   * to be removed from the remember set.
   */
  void WriteBarrier::unremember_object(Object* target) {
    thread::SpinLock::LockGuard lg(lock_);

    for(ObjectArray::iterator oi = remember_set_->begin();
        oi != remember_set_->end();
        oi++) {
      if(*oi == target) {
        *oi = NULL;
        target->clear_remember();
      }
    }
  }

  /**
   * Returns the current contents of the remember set, and then sets up a new,
   * empty remember set.
   * Note: It is the responsibility of the caller to dispose of the returned
   * ObjectArray.
   *
   * /returns an ObjectArray instance containing the remember set contents
   * prior to this method being called.
   */
  ObjectArray* WriteBarrier::swap_remember_set() {
    thread::SpinLock::LockGuard lg(lock_);

    ObjectArray* cur = remember_set_;
    remember_set_ = new ObjectArray(0);
    return cur;
  }

}}
