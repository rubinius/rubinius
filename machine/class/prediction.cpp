#include "object_utils.hpp"
#include "memory.hpp"

#include "class/executable.hpp"
#include "class/module.hpp"
#include "class/prediction.hpp"

namespace rubinius {
  void Prediction::bootstrap(STATE) {
    GO(prediction).set(state->memory()->new_class<Class, Prediction>(
          state, G(rubinius), "Prediction"));
  }

  Prediction* Prediction::create(STATE) {
    return state->memory()->new_object<Prediction>(state, G(prediction));
  }

  Object* Prediction::valid_p(STATE) {
    return RBOOL(valid() == valid_prediction);
  }
}
