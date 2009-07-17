#include "builtin/class.hpp"
#include "builtin/data.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    void flush_cached_rdata(NativeMethodEnvironment* env, Handle* handle) {
      if(handle->is_rdata()) {
        Data* data = c_as<Data>(handle->object());
        RData* rdata = handle->as_rdata(env);

        data->mark(env->state(), rdata->dmark);
        data->free(env->state(), rdata->dfree);
        data->data(env->state(), rdata->data);
      }
    }

    RData* Handle::as_rdata(NativeMethodEnvironment* env) {
      if(type_ != cRData) {
        Data* data = c_as<Data>(object());

        RData* d = new RData;
        d->dmark = data->mark();
        d->dfree = data->free();
        d->data = data->data();

        type_ = cRData;
        as_.rdata = d;

        flush_ = flush_cached_rdata;

        env->state()->shared.global_handles()->move(this,
            env->state()->shared.cached_handles());
      }

      return as_.rdata;
    }
  }
}

extern "C" {
  struct RData* capi_rdata_struct(VALUE data_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Handle* handle = Handle::from(data_handle);
    return handle->as_rdata(env);
  }

  VALUE rb_data_object_alloc(VALUE klass, void* ptr,
      RUBY_DATA_FUNC mark, RUBY_DATA_FUNC free) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Class* data_klass = c_as<Class>(env->get_object(klass));

    Data* data = Data::create(env->state(), ptr, mark, free);

    data->klass(env->state(), data_klass);

    return env->get_handle(data);
  }
}
