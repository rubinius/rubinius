#ifndef RBX_BUILTIN_WEAKREF_HPP
#define RBX_BUILTIN_WEAKREF_HPP

#include "object_utils.hpp"
#include "type_info.hpp"

#include "class/object.hpp"
#include "class/exception.hpp"

namespace rubinius {
  class Memory;

  class WeakRef : public Object {
  public:
    const static object_type type = WeakRefType;

  private:
    attr_field(object, Object*);

  public:

    static void bootstrap(STATE);
    static void initialize(STATE, WeakRef* obj) {
      obj->object(cNil);
    }

    // Rubinius.primitive+ :weakref_object
    Object* get_object() const {
      return object();
    }

    // Rubinius.primitive+ :weakref_set_object
    Object* set_object(STATE, Object* obj) {
      object(obj);
      return obj;
    }

    // Rubinius.primitive+ :weakref_new
    static WeakRef* create(STATE, Object* obj);

    bool alive_p() const {
      return object()->reference_p();
    }

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {}
      virtual void update_weakref(STATE, Object* obj);
    };
  };
}

#endif
