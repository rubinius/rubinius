#include "builtin/class.hpp"
#include "builtin/data.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

namespace rubinius {
  namespace capi {
    void capi_rdata_flush(NativeMethodEnvironment* env,
        CApiStructs& data_structs, bool release_memory) {
      Data* data;
      struct RData* d = 0;

      for(CApiStructs::iterator iter = data_structs.begin();
          iter != data_structs.end();
          iter++) {
        data = c_as<Data>(env->get_object(iter->first));
        d = (struct RData*)iter->second;

        data->mark(env->state(), d->dmark);
        data->free(env->state(), d->dfree);
        data->data(env->state(), d->data);

        if(release_memory) delete d;
      }
    }
  }
}

extern "C" {
  struct RData* capi_rdata_struct(VALUE data_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    CApiStructs& data_structs = env->data();
    CApiStructs::iterator iter = data_structs.find(data_handle);
    if(iter != data_structs.end()) {
      return (struct RData*)iter->second;
    }

    Data* data = c_as<Data>(env->get_object(data_handle));

    struct RData* d = new struct RData;
    d->dmark = data->mark();
    d->dfree = data->free();
    d->data = data->data();

    data_structs[data_handle] = d;

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
