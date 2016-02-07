#ifndef RBX_BUILTIN_WEAKREF_HPP
#define RBX_BUILTIN_WEAKREF_HPP

#include "object_utils.hpp"
#include "type_info.hpp"

#include "builtin/object.hpp"
#include "builtin/exception.hpp"

namespace rubinius {
  class ObjectMemory;

  class WeakRef : public Object {
  public:
    const static object_type type = WeakRefType;

  private:
    Object* object_;

  public:

    static void bootstrap(STATE);
    static void initialize(STATE, WeakRef* obj) {
      obj->object_ = nil<Object>();
    }

    // Rubinius.primitive+ :weakref_object
    Object* object() const {
      return object_;
    }

    void set_object(ObjectMemory* om, Object* obj);

    // Rubinius.primitive+ :weakref_set_object
    Object* set_object(STATE, Object* obj);

    // Rubinius.primitive+ :weakref_new
    static WeakRef* create(STATE, Object* obj);

    bool alive_p() const {
      return object_->reference_p();
    }

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(Object* obj, memory::ObjectMark& mark);
      virtual void auto_mark(Object* obj, memory::ObjectMark& mark) {}
    };
  };
}

#endif
