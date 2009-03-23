#include "builtin/class.hpp"
#include "builtin/data.hpp"

#include "capi/capi.hpp"

using namespace capi;

extern "C" {
  // Used in a macro, has to be visible in C
  void** capi_data_ptr_get_address(VALUE data_handle) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Data* data = c_as<Data>(env->get_object(data_handle));

    return data->data_address();
  }

  VALUE rb_data_object_alloc(VALUE klass, RUBY_DATA_FUNC mark,
                             RUBY_DATA_FUNC free, void* ptr) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    Class* data_klass = c_as<Class>(env->get_object(klass));

    Data* data = Data::create(env->state(), ptr, mark, free);

    data->klass(env->state(), data_klass);

    return env->get_handle(data);
  }
}
