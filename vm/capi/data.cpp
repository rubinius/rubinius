#include "objectmemory.hpp"

#include "builtin/class.hpp"
#include "builtin/data.hpp"

#include "capi/capi.hpp"
#include "capi/18/include/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
    RData*  Handle::as_rdata(NativeMethodEnvironment* env) {
      Data* data = as<Data>(object_);

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
}
