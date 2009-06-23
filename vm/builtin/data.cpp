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
    data = state->new_object<Data>(G(data));

    data->data_ = data_ptr;
    data->mark_ = mark;
    data->free_ = free;

    return data;
  }

  void Data::Info::mark(Object* t, ObjectMark& mark) {
    auto_mark(t, mark);

    Data* data = as<Data>(t);

    if(data->mark_) {
      ObjectMark* cur = VM::current_state()->current_mark;
      VM::current_state()->current_mark = &mark;
      (*data->mark_)(data->data_);
      VM::current_state()->current_mark = cur;
    }
  }

}
