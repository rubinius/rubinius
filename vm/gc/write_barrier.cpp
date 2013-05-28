#include "builtin/object.hpp"
#include "gc/write_barrier.hpp"
#include "object_utils.hpp"

namespace rubinius {
namespace gc {

  /**
   * Creates a remember set for the new WriteBarrier.
   */
  WriteBarrier::WriteBarrier()
    : remember_set_(new ObjectArray(0))
    , marked_set_(new ObjectArray(0))
  {}

  /**
   * Deletes the remember set associated with this WriteBarrier.
   */
  WriteBarrier::~WriteBarrier() {
    delete remember_set_;
    delete marked_set_;
  }

  /**
   * Store the mature Object target into the remember set.
   * Called when it is determined (elsewhere) that the target object holds
   * one or more references to objects currently in the young generation.
   *
   * @param target The mature object to be added to the remember set.
   */
  void WriteBarrier::remember_object(Object* target) {
    utilities::thread::SpinLock::LockGuard lg(lock_);

    // If it's already remembered, ignore this request
    if(target->remembered_p()) return;
    target->set_remember();
    remember_set_->push_back(target);
  }

  /**
   * Called during collection of the mature generation to remove the mature
   * object target from the remember set (if present).
   * When a mature collection is performed, any mature object that is collected
   * needs to be removed from the remember set, to ensure it does not keep
   * alive young generation objects that are no longer reachable.
   *
   * Note: This method performs a linear scan for the remember object in the
   * remember set, so is not efficient for large numbers of removals. The
   * ImmixGC thus clears the remember set directly in a single pass using
   * #unremember_objects.
   *
   * @todo Should the MarkSweepGC use the same method?
   *
   * @param target The mature object being collected (deleted), and therefore
   * to be removed from the remember set.
   */
  void WriteBarrier::unremember_object(Object* target) {
    utilities::thread::SpinLock::LockGuard lg(lock_);

    for(ObjectArray::iterator oi = remember_set_->begin();
        oi != remember_set_->end();
        ++oi) {
      if(*oi == target) {
        *oi = NULL;
        target->clear_remember();
      }
    }
  }

  /**
   * Removes objects in the remember set that do not have the specified mark.
   *
   * @param mark The mark bit pattern used on the last trace of live objects.
   */
  int WriteBarrier::unremember_objects(unsigned int mark) {
    utilities::thread::SpinLock::LockGuard lg(lock_);
    int cleared = 0;

    for(ObjectArray::iterator oi = remember_set_->begin();
        oi != remember_set_->end();
        ++oi) {
      Object* tmp = *oi;
      // unremember_object throws a NULL in to remove an object
      // so we don't have to compact the set in unremember
      if(tmp && !tmp->marked_p(mark)) {
        cleared++;
        *oi = NULL;
      }
    }
    return cleared;
  }

  /**
   * Store the object in the set to be marked.
   * Called when it is determined (elsewhere) that the value is stored
   * in a target object that is already scanned.
   *
   * @param target The object to be added to the mark set.
   */
  void WriteBarrier::mark_object(Object* val) {
    utilities::thread::SpinLock::LockGuard lg(lock_);
    marked_set_->push_back(val);
  }

  /**
   * Returns the current contents of the remember set, and then sets up a new,
   * empty remember set.
   * Note: It is the responsibility of the caller to dispose of the returned
   * ObjectArray.
   *
   * @returns an ObjectArray instance containing the remember set contents
   * prior to this method being called.
   */
  ObjectArray* WriteBarrier::swap_remember_set() {
    utilities::thread::SpinLock::LockGuard lg(lock_);

    ObjectArray* cur = remember_set_;
    remember_set_ = new ObjectArray(0);
    return cur;
  }

  /**
   * Returns the current contents of the remember set, and then sets up a new,
   * empty remember set.
   * Note: It is the responsibility of the caller to dispose of the returned
   * ObjectArray.
   *
   * @returns an ObjectArray instance containing the remember set contents
   * prior to this method being called.
   */
  ObjectArray* WriteBarrier::swap_marked_set() {
    utilities::thread::SpinLock::LockGuard lg(lock_);

    ObjectArray* cur = marked_set_;
    marked_set_ = new ObjectArray(0);
    return cur;
  }

}}
