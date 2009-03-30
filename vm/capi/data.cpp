#include "builtin/class.hpp"
#include "builtin/data.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
    void capi_rdata_flush() {
      NativeMethodEnvironment* env = NativeMethodEnvironment::get();

      Data* data;
      struct RData* d = 0;
      CApiStructs& data_str = env->data();

      for(CApiStructs::iterator iter = data_str.begin();
          iter != data_str.end();
          iter++) {
        data = c_as<Data>(env->get_object(iter->first));
        d = (struct RData*)iter->second;

        data->mark(env->state(), d->dmark);
        data->free(env->state(), d->dfree);
        data->data(env->state(), d->data);

        delete d;
      }
    }
  }
}

extern "C" {
  struct RData* capi_rdata_struct(VALUE data_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    CApiStructs& data_str = env->data();
    CApiStructs::iterator iter = data_str.find(data_handle);
    if(iter != data_str.end()) {
      return (struct RData*)iter->second;
    }

    Data* data = c_as<Data>(env->get_object(data_handle));

    struct RData* d = new struct RData;
    d->dmark = data->mark();
    d->dfree = data->free();
    d->data = data->data();

    data_str[data_handle] = d;

    return d;
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
