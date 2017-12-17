#ifndef RBX_CLASS_PREDICTION_HPP
#define RBX_CLASS_PREDICTION_HPP

#include "class/object.hpp"

namespace rubinius {
  class Prediction : public Object {
  public:
    const static object_type type = PredictionType;

    attr_field(valid, intptr_t);

    static void bootstrap(STATE);
    static void initialize(STATE, Prediction* obj) {
      obj->valid(true);
    }

    static Prediction* create(STATE);

    class Info : public TypeInfo {
      public:
        Info(object_type type)
          : TypeInfo(type)
        {
          allow_user_allocate = false;
        }

        void mark(Object* obj, memory::ObjectMark& mark) { }
        void auto_mark(Object* obj, memory::ObjectMark& mark) { }
        void set_field(STATE, Object* target, size_t index, Object* val) { }
        Object* get_field(STATE, Object* target, size_t index) { return cNil; }
        void populate_slot_locations() { }
    };
  };
}

#endif
