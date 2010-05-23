#include "capi/capi.hpp"
#include "capi/include/ruby.h"

#include "builtin/float.hpp"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    Float* capi_get_float(NativeMethodEnvironment* env, VALUE float_handle) {
      Handle* handle = Handle::from(float_handle);
      handle->flush(env);
      return as<Float>(handle->object());
    }

    void flush_cached_rfloat(NativeMethodEnvironment* env, Handle* handle) {
      if(handle->is_rfloat()) {
        Float* obj = c_as<Float>(handle->object());
        RFloat* rfloat = handle->as_rfloat(env);
        obj->val = rfloat->value;
      }
    }

    RFloat* Handle::as_rfloat(NativeMethodEnvironment* env) {
      if(type_ != cRFloat) {
        Float* float_obj = c_as<Float>(object());

        RFloat* f = new RFloat;
        f->value = float_obj->val;

        type_ = cRFloat;
        as_.rfloat = f;

        flush_ = flush_cached_rfloat;

        env->state()->shared.global_handles()->move(this,
            env->state()->shared.cached_handles());
      }

      return as_.rfloat;
    }
  }
}

extern "C" {
  struct RFloat* capi_rfloat_struct(VALUE float_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return Handle::from(float_handle)->as_rfloat(env);
  }

  VALUE rb_float_new(double val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return env->get_handle(Float::create(env->state(), val));
  }
  
  VALUE rb_Float(VALUE object_handle) {
    return rb_convert_type(object_handle, 0, "Float", "to_f");
  }
}
