#include "builtin/object.hpp"
#include "gc/write_barrier.hpp"

namespace rubinius {
namespace gc {
  WriteBarrier::WriteBarrier()
    : remember_set_(new ObjectArray(0))
  {}

  WriteBarrier::~WriteBarrier() {
    delete remember_set_;
  }

  /* Store an object into the remember set. Called when we've calculated
   * externally that the object in question needs to be remembered */
  void WriteBarrier::remember_object(Object* target) {
    thread::SpinLock::LockGuard lg(lock_);

    assert(target->zone() == MatureObjectZone);

    /* If it's already remembered, ignore this request */
    if(target->remembered_p()) return;
    target->set_remember();
    remember_set_->push_back(target);
  }

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

  ObjectArray* WriteBarrier::swap_remember_set() {
    thread::SpinLock::LockGuard lg(lock_);

    ObjectArray* cur = remember_set_;
    remember_set_ = new ObjectArray(0);
    return cur;
  }

}}
