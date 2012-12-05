#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "builtin/object.hpp"

#include "builtin/reflector.hpp"

#include "ontology.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"
#include "primitives.hpp"

namespace rubinius {
  void Reflector::init(STATE) {
    GO(reflector).set(ontology::new_class(state, "Reflector",
                      G(object), G(rubinius)));
    G(reflector)->set_object_type(state, Reflector::type);
  }

  Reflector* Reflector::create(STATE, Object* target) {
    Reflector* ref = state->new_object<Reflector>(G(reflector));
    ref->target(state, target);
    return ref;
  }

  Object* Reflector::get(STATE, Symbol* name) {
    if(name->is_ivar_p(state)->false_p()) return cNil;
    return target_->get_ivar(state, name);
  }

  Object* Reflector::set(STATE, Symbol* name, Object* val) {
    if(name->is_ivar_p(state)->false_p()) {
      return Primitives::failure();
    }

    target_->check_frozen(state);
    target_->set_ivar(state, name, val);

    return val;
  }
}
