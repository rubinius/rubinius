#include "builtin/data.hpp"

namespace rubinius {

  void Data::init(STATE) {
    GO(data).set(state->new_class("Data", G(object), Data::fields));
    G(data)->set_object_type(state, DataType);
  }

  Data* create(STATE, void* data_ptr, void (*mark) (void*),
               void (*free) (void*)) {
    Data* data;
    data = (Data*)state->om->new_object(G(data), Data::fields);

    data->data_ = data_ptr;
    data->mark_ = mark;
    data->free_ = free;

    return data;
  }

  void Data::Info::mark(Object* t, ObjectMark& mark) {
  }

}
