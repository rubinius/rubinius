#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "gc/gc.hpp"
#include "builtin/object.hpp"
#include "builtin/class.hpp"
#include "builtin/call_site.hpp"
#include "ontology.hpp"

namespace rubinius {

  void CallSite::init(STATE) {
    GO(call_site).set(ontology::new_class(state, "CallSite", G(object), G(rubinius)));
    G(executable)->set_object_type(state, CallSiteType);
  }

  void CallSite::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
  }

}

