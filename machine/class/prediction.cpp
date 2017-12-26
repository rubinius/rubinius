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

  void MethodPrediction::bootstrap(STATE) {
    GO(method_prediction).set(state->memory()->new_class<Class, MethodPrediction>(
          state, G(rubinius), "MethodPrediction"));
  }

  MethodPrediction* MethodPrediction::create(STATE, Module* module, Executable* executable) {
    MethodPrediction* pred =
      state->memory()->new_object<MethodPrediction>(state, G(method_prediction));

    pred->module(state, module);
    pred->executable(state, executable);

    return pred;
  }
}
