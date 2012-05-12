#ifndef RBX_BUILTIN_WEAKREF_HPP
#define RBX_BUILTIN_WEAKREF_HPP

#include "builtin/object.hpp"
#include "builtin/exception.hpp"
#include "type_info.hpp"

namespace rubinius {
  class WeakRef : public Object {
  public:
    const static object_type type = WeakRefType;

  private:
    Object* object_;
    bool reference_;

  public:

    // Rubinius.primitive+ :weakref_object
    Object* object() {
      return object_;
    }

    void update_object(gc::WriteBarrier* wb, Object* obj) {
      object_ = obj;
      write_barrier(wb, obj);
    }

    // Rubinius.primitive :weakref_set_object
    Object* set_object(STATE, Object* obj) {
      object_ = obj;
      reference_ = obj->reference_p();
      write_barrier(state, obj);
      return obj;
    }

    static void init(STATE);

    // Rubinius.primitive+ :weakref_new
    static WeakRef* create(STATE, Object* obj);


    bool reference_alive_p() {
      return reference_ && object_->reference_p();
    }

    // Rubinius.primitive :weakref_alive_p
    Object* alive_p() {
      if(reference_ && !object_->reference_p()) {
        return cFalse;
      } else {
        return cTrue;
      }
    }

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(Object* obj, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };
}

#endif
