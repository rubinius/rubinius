#include "builtin/data.hpp"
#include "builtin/class.hpp"
#include "objectmemory.hpp"
#include "object_utils.hpp"

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
    assert(data->pin());

    data->data(state, data_ptr);
    data->mark(state, mark);
    data->free(state, free);

    return data;
  }

  void Data::Info::mark(Object* t, ObjectMark& mark) {
    auto_mark(t, mark);

    Data* data = as<Data>(t);

    if(data->mark()) {
      ObjectMark* cur = VM::current_state()->current_mark;
      VM::current_state()->current_mark = &mark;
      (*data->mark())(data->data());
      VM::current_state()->current_mark = cur;
    }
  }

}
