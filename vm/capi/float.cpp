#include "capi/capi.hpp"
#include "capi/ruby.h"

#include "builtin/float.hpp"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    Float* capi_get_float(VALUE float_handle) {
      Handle* handle = Handle::from(float_handle);
      Float* float_obj = c_as<Float>(handle->object());
      if(handle->is_rfloat()) {
        // Flushing value from the RFloat back to the Float object
        RFloat* f = handle->as_rfloat();
        float_obj->val = f->value;
      }

      return float_obj;
    }

    RFloat* Handle::as_rfloat() {
      if(type_ != cRFloat) {
        Float* float_obj = c_as<Float>(object());

        RFloat* f = new RFloat;
        f->value = float_obj->val;

        type_ = cRFloat;
        as_.rfloat = f;
      }

      return as_.rfloat;
    }
  }
}

extern "C" {
  struct RFloat* capi_rfloat_struct(VALUE float_handle) {
    Handle* handle = Handle::from(float_handle);
    return handle->as_rfloat();
  }

  VALUE rb_float_new(double val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return env->get_handle(Float::create(env->state(), val));
  }
}
