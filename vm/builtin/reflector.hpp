#ifndef RBX_BUILTIN_REFLECTOR_HPP
#define RBX_BUILTIN_REFLECTOR_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

#include "virtual.hpp" // ObjectCallback
#include "gc/root.hpp" // TypedRoot

#include "builtin/thread.hpp"

namespace rubinius {
  class Reflector : public Object {
  public:
    static void init(STATE);

    const static object_type type = ReflectorType;

  private:
    Object* target_;

  public:
    attr_accessor(target, Object);

    // Rubinius.primitive :reflector_create
    static Reflector* create(STATE, Object* obj);

    // Rubinius.primitive :reflector_get
    Object* get(STATE, Symbol* name);

    // Rubinius.primitive :reflector_set
    Object* set(STATE, Symbol* name, Object* val);
  };
}

#endif
