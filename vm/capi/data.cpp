#include "object_memory.hpp"

#include "builtin/class.hpp"
#include "builtin/data.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
    RData*  Handle::as_rdata(NativeMethodEnvironment* env) {
      Data* data = c_as<Data>(object_);

      if(data->freed_p()) {
        rb_raise(rb_eArgError, "Data object has already been freed");
      }

      if(type_ == cRData) {
        return as_.rdata;
      } else {
        type_ = cRData;

        RData* rdata = new RData;
        rdata->data = 0;
        rdata->dmark = 0;
        rdata->dfree = 0;

        as_.rdata = rdata;

        return rdata;
      }
    }

    RTypedData*  Handle::as_rtypeddata(NativeMethodEnvironment* env) {
      Data* data = c_as<Data>(object_);

      if(data->freed_p()) {
        rb_raise(rb_eArgError, "Data object has already been freed");
      }

      if(type_ == cRData) {
        return as_.rtypeddata;
      } else {
        type_ = cRData;

        RTypedData* rtypeddata = new RTypedData;
        rtypeddata->data = 0;
        rtypeddata->type = 0;
        rtypeddata->typed_flag = 1;

        as_.rtypeddata = rtypeddata;

        return rtypeddata;
      }
    }
  }
}

extern "C" {
  struct RData* capi_rdata_struct(VALUE data_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Handle* handle = Handle::from(data_handle);
    env->check_tracked_handle(handle, false);

    return handle->as_rdata(env);
  }

  VALUE rb_data_object_alloc(VALUE klass, void* ptr,
      RUBY_DATA_FUNC mark, RUBY_DATA_FUNC free) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(!klass) klass = rb_cObject;

    Class* data_klass = c_as<Class>(env->get_object(klass));

    Data* data = Data::create(env->state(), ptr, mark, free);

    data->klass(env->state(), data_klass);

    return env->get_handle(data);
  }

  struct RTypedData* capi_rtypeddata_struct(VALUE data_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Handle* handle = Handle::from(data_handle);
    env->check_tracked_handle(handle, false);

    return handle->as_rtypeddata(env);
  }

  VALUE rb_data_typed_object_alloc(VALUE klass, void* ptr, const rb_data_type_t* type) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(!klass) klass = rb_cObject;

    Class* data_klass = c_as<Class>(env->get_object(klass));

    Data* data = Data::create_typed(env->state(), ptr, reinterpret_cast<const struct rb_data_type_struct_shadow*>(type));

    data->klass(env->state(), data_klass);

    return env->get_handle(data);
  }

  int rb_typeddata_inherited_p(const rb_data_type_t *child, const rb_data_type_t *parent) {
    while (child) {
      if (child == parent) return 1;
      child = child->parent;
    }
    return 0;
  }

  void* rb_check_typeddata(VALUE obj, const rb_data_type_t* data_type) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    const char* mesg  = "wrong argument type %s (expected %s)";
    Data* data = c_as<Data>(env->get_object(obj));
    if(!data->typed()) {
      rb_raise(rb_eTypeError, mesg, rb_obj_classname(obj), data_type->wrap_struct_name);
    }

    const rb_data_type_t* other_type = reinterpret_cast<const rb_data_type_t*>(data->data_type());

    if (!rb_typeddata_inherited_p(other_type, data_type)) {
      rb_raise(rb_eTypeError, mesg, other_type->wrap_struct_name, data_type->wrap_struct_name);
    }
    return data->data();
  }

  int rb_typeddata_is_kind_of(VALUE obj, const rb_data_type_t *data_type) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    Data* data = try_as<Data>(env->get_object(obj));
    if(!data || !data->typed()) return 0;
    return rb_typeddata_inherited_p(RTYPEDDATA_TYPE(obj), data_type);
  }
}
