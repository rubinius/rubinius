#include "gc/root.hpp"
#include "vm.hpp"

namespace rubinius {

/* Roots */

  Root* Roots::front() {
    return static_cast<Root*>(head());
  }


/* Root */

  Root::Root(STATE):
    LinkedList::Node(), object(NULL), roots(&state->globals.roots)
  { }

  Root::Root(STATE, Object* obj):
    LinkedList::Node(), object(NULL), roots(NULL)
  {
    set(obj, &state->globals.roots);
  }

  void Root::set(Object* obj, Roots* r) {
    // Still in the same set, no problem, just repoint
    // object.
    if(roots == r) {

      // We don't add the root until it's got an object.
      if(!object && obj) roots->add(this);
      object = obj;

    // Moving to a new set. Remove ourselves from
    // the current set if we added ourself (we have an
    // object)
    } else {
      if(object) roots->remove(this);
      object = obj;
      roots = r;
      if(object) roots->add(this);
    }
  }
}
