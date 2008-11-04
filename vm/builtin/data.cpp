#include "builtin/data.hpp"
#include "builtin/class.hpp"
#include "objectmemory.hpp"

namespace rubinius {

  void Data::init(STATE) {
    GO(data).set(state->new_class("Data", G(object), Data::fields));
    G(data)->set_object_type(state, DataType);
  }

  Data* Data::create(STATE, void* data_ptr, void (*mark) (void*),
                     void (*free) (void*)) {
    Data* data;
    data = (Data*)state->om->new_object(G(data), Data::fields);

    data->data_ = data_ptr;
    data->mark_ = mark;
    data->free_ = free;
    data->mark_context = NativeMethodContext::current();

    return data;
  }

  void Data::Info::mark(Object* t, ObjectMark& mark) {
    auto_mark(t, mark); // HACK should be super() in case mark() ever changes

    Data* data = as<Data>(t);

    if(data->mark_) {
      NativeMethodContext* orig_current = NativeMethodContext::current();
      NativeMethodContext::current_context_is(data->mark_context);
      VM::current_state()->current_mark = mark;

      (*data->mark_)((void*)data->mark_context->handle_for(data));

      VM::current_state()->current_mark = NULL;
      NativeMethodContext::current_context_is(orig_current);
    }
  }

}
