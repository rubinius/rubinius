#include "builtin/data.hpp"
#include "builtin/class.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"

#include "gc/gc.hpp"

#include "capi/capi.hpp"

namespace rubinius {

  void Data::init(STATE) {
    GO(data).set(state->new_class("Data", G(object)));
    G(data)->set_object_type(state, DataType);
  }

  Data* Data::create(STATE, void* data_ptr, MarkFunctor mark, FreeFunctor free) {
    Data* data;

    // Construct this object pinned, so that we can pass out address to the
    // inside of it to C code without worry about object movement.
    data = state->new_object_mature<Data>(G(data));
    if(!data->pin()) { abort(); }

    data->data(state, data_ptr);
    data->mark(state, mark);
    data->free(state, free);

    return data;
  }

  void Data::finalize(STATE, Data* data) {
    FreeFunctor f = data->free();
    if(f) f(data->data());
  }

  void Data::Info::mark(Object* t, ObjectMark& mark) {
    auto_mark(t, mark);

    Data* data = as<Data>(t);

    if(data->mark()) {
      ObjectMark* cur = capi::current_mark();
      capi::set_current_mark(&mark);

      (*data->mark())(data->data());

      capi::set_current_mark(cur);
    }

    // Data's can be written to without write barriers running,
    // if it lives to here, we need to always check it.
    mark.gc->state()->om->remember_object(t);
  }

}
