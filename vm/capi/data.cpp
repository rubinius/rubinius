#include "builtin/class.hpp"
#include "builtin/data.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {

    void capi_rdata_flush_handle(NativeMethodEnvironment* env, Handle* handle) {
      Data* data = c_as<Data>(handle->object());

      if(handle->is_rdata()) {
        RData* d = handle->as_rdata(env);

        data->mark(env->state(), d->dmark);
        data->free(env->state(), d->dfree);
        data->data(env->state(), d->data);
      }
    }

    void capi_rdata_flush(NativeMethodEnvironment* env,
        CApiStructs& data_structs, bool release_memory) {
      Data* data;
      struct RData* d = 0;

      for(CApiStructs::iterator iter = data_structs.begin();
          iter != data_structs.end();
          iter++) {
        Handle* handle = iter->first;
        data = c_as<Data>(handle->object());

        if(handle->is_rdata()) {
          d = handle->as_rdata(env);

          data->mark(env->state(), d->dmark);
          data->free(env->state(), d->dfree);
          data->data(env->state(), d->data);

          if(release_memory) handle->free_data();
        }
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
