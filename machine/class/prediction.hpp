#ifndef RBX_CLASS_PREDICTION_HPP
#define RBX_CLASS_PREDICTION_HPP

#include "class/object.hpp"

#include <atomic>

namespace rubinius {
  class Executable;
  class Module;

  class Prediction : public Object {
  public:
    const static object_type type = PredictionType;
    const static uint64_t valid_prediction = 0xffffffffffffffffULL;

    std::atomic<uint64_t> _valid_;

    uint64_t valid() {
      return _valid_.load(std::memory_order_seq_cst);
    }

    void valid(uint64_t value) {
      _valid_.store(value, std::memory_order_seq_cst);
    }

    static void bootstrap(STATE);
    static void initialize(STATE, Prediction* obj) {
      obj->valid(valid_prediction);
    }

    static Prediction* create(STATE);

    void invalidate() {
      valid(0);
    }

    // Rubinius.primitive :prediction_valid_p
    Object* valid_p(STATE);

    class Info : public TypeInfo {
      public:
        Info(object_type type)
          : TypeInfo(type)
        {
          allow_user_allocate = false;
        }

        virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) { }
        virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) { }
        virtual void set_field(STATE, Object* target, size_t index, Object* val) { }
        virtual Object* get_field(STATE, Object* target, size_t index) { return cNil; }
        virtual void populate_slot_locations() { }
    };
  };
}

#endif
