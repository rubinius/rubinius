#include "memory.hpp"
#include "capi/capi.hpp"
#include "capi/ruby.h"

#include "class/float.hpp"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  RFloat* MemoryHandle::get_rfloat(STATE) {
    if(rfloat_p()) {
      RFloat* rfloat = reinterpret_cast<RFloat*>(data());
      rfloat->value = c_as<Float>(object())->value();
      return rfloat;
    } else if(object_type_p()) {
      RFloat* rfloat = new RFloat();
      rfloat->value = c_as<Float>(object())->value();
      set_rfloat(rfloat);
      return rfloat;
    } else {
      Exception::raise_runtime_error(state, "C-API handle invalid reference as RFloat");
    }
  }
}

extern "C" {
  struct RFloat* capi_rfloat_struct(VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    return MemoryHandle::from(value)->get_rfloat(env->state());
  }

  double capi_rfloat_value(VALUE value) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Float* f = MemoryHandle::object<Float>(value);
    return f->to_double(env->state());
  }

  VALUE rb_float_new(double val) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    return MemoryHandle::value(Float::create(env->state(), val));
  }
}
