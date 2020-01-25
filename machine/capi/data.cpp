#include "memory.hpp"

#include "class/class.hpp"
#include "class/data.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  RData* MemoryHandle::get_rdata(STATE) {
    if(rdata_p()) {
      return reinterpret_cast<RData*>(data());
    } else if(object_type_p()) {
      RData* rdata = new RData;
      rdata->data = 0;
      rdata->dmark = 0;
      rdata->dfree = 0;

      set_rdata(rdata);
      return rdata;
    } else {
      Exception::raise_runtime_error(state, "C-API handle invalid reference as RData");
    }
  }

  RTypedData* MemoryHandle::get_rtypeddata(STATE) {
    if(rtypeddata_p()) {
      return reinterpret_cast<RTypedData*>(data());
    } else if(object_type_p()) {
      RTypedData* rtypeddata = new RTypedData;
      rtypeddata->data = 0;
      rtypeddata->type = 0;
      rtypeddata->typed_flag = 1;

      set_rtypeddata(rtypeddata);
      return rtypeddata;
    } else {
      Exception::raise_runtime_error(state, "C-API handle invalid reference as RData");
    }
  }
}

extern "C" {
  struct RData* capi_rdata_struct(VALUE data_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    MemoryHandle* handle = MemoryHandle::from(data_handle);

    if(RData* rdata = handle->get_rdata(env->state())) {
      return rdata;
    } else {
      rdata = new RData();
      handle->set_rdata(rdata);
      return rdata;
    }
  }

  VALUE rb_data_object_alloc(VALUE klass, void* ptr,
      RUBY_DATA_FUNC mark, RUBY_DATA_FUNC free) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(!klass) klass = rb_cObject;

    Class* data_klass = MemoryHandle::object<Class>(klass);

    Data* data = Data::create(env->state(), ptr, mark, free);

    data->klass(env->state(), data_klass);

    return MemoryHandle::value(data);
  }

  struct RTypedData* capi_rtypeddata_struct(VALUE data_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    MemoryHandle* handle = MemoryHandle::from(data_handle);

    if(RTypedData* rdata = handle->get_rtypeddata(env->state())) {
      return rdata;
    } else {
      rdata = new RTypedData();
      handle->set_rtypeddata(rdata);
      return rdata;
    }
  }

  VALUE rb_data_typed_object_alloc(VALUE klass, void* ptr, const rb_data_type_t* type) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(!klass) klass = rb_cObject;

    Class* data_klass = MemoryHandle::object<Class>(klass);

    Data* data = Data::create_typed(env->state(), ptr, reinterpret_cast<const struct rb_data_type_struct_shadow*>(type));

    data->klass(env->state(), data_klass);

    return MemoryHandle::value(data);
  }

  int rb_typeddata_inherited_p(const rb_data_type_t *child, const rb_data_type_t *parent) {
    while (child) {
      if (child == parent) return 1;
      child = child->parent;
    }
    return 0;
  }

  void* rb_check_typeddata(VALUE obj, const rb_data_type_t* data_type) {
    const char* mesg  = "wrong argument type %s (expected %s)";
    Data* data = MemoryHandle::object<Data>(obj);
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
    Data* data = MemoryHandle::try_as<Data>(obj);
    if(!data || !data->typed()) return 0;
    return rb_typeddata_inherited_p(RTYPEDDATA_TYPE(obj), data_type);
  }
}
