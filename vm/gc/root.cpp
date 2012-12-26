#include "gc/root.hpp"
#include "vm.hpp"

namespace rubinius {

/* Roots */

  Root* Roots::front() {
    return static_cast<Root*>(head());
  }

  void Roots::add(Root* node) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    this->LinkedList::add(node);
  }

  void Roots::remove(Root* node) {
    utilities::thread::SpinLock::LockGuard guard(lock_);
    this->LinkedList::remove(node);
  }

/* Root */

  Root::Root(STATE)
    : LinkedList::Node()
    , roots_(&state->globals().roots)
    , object_(cUndef)
  {}

  Root::Root(STATE, Object* obj)
    : LinkedList::Node()
    , roots_(NULL)
    , object_(cUndef)
  {
    set(obj, &state->globals().roots);
  }

  Root::Root(VM* vm)
    : LinkedList::Node()
    , roots_(&vm->globals().roots)
    , object_(cUndef)
  {}

  Root::Root(VM* vm, Object* obj)
    : LinkedList::Node()
    , roots_(NULL)
    , object_(cUndef)
  {
    set(obj, &vm->globals().roots);
  }

  void Root::set(Object* obj, Roots* r) {
    // Still in the same set, no problem, just repoint
    // object.
    if(roots_ == r) {

      // We don't add the root until it's got an object.
      if(object_ == cUndef && obj && obj != cUndef) roots_->add(this);
      object_ = obj;

    // Moving to a new set. Remove ourselves from
    // the current set if we added ourself (we have an
    // object)
    } else {
      if(object_ != cUndef) roots_->remove(this);
      object_ = obj;
      roots_ = r;
      if(object_ != cUndef) roots_->add(this);
    }
  }
}
