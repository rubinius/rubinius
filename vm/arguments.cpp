#include "arguments.hpp"
#include "builtin/array.hpp"

namespace rubinius {
  void Arguments::append(STATE, Array* ary) {
    Tuple* tup = Tuple::create(state, ary->size() + total());

    for(uint32_t i = 0; i < total(); i++) {
      tup->put(state, i, get_argument(i));
    }

    for(uint32_t i = 0, n = total(); i < ary->size(); i++, n++) {
      tup->put(state, n, ary->get(state, i));
    }

    use_tuple(tup, total() + ary->size());
  }

  void Arguments::prepend(STATE, Array* ary) {
    Tuple* tup = Tuple::create(state, ary->size() + total());

    for(uint32_t i = 0; i < ary->size(); i++) {
      tup->put(state, i, ary->get(state, i));
    }

    for(uint32_t i = 0, n = ary->size(); i < total(); i++, n++) {
      tup->put(state, n, get_argument(i));
    }

    use_tuple(tup, total() + ary->size());
  }

  Array* Arguments::as_array(STATE) {
    Array* ary = Array::create(state, total());

    for(uint32_t i = 0; i < total(); i++) {
      ary->set(state, i, get_argument(i));
    }

    return ary;
  }

  void Arguments::unshift(STATE, Object* val) {
    Tuple* tup = Tuple::create(state, total() + 1);

    tup->put(state, 0, val);

    for(uint32_t i = 0; i < total(); i++) {
      tup->put(state, i + 1, get_argument(i));
    }

    use_tuple(tup, total() + 1);
  }

  void Arguments::unshift2(STATE, Object* one, Object* two) {
    Tuple* tup = Tuple::create(state, total() + 2);

    tup->put(state, 0, one);
    tup->put(state, 1, two);

    for(uint32_t i = 0; i < total(); i++) {
      tup->put(state, i + 2, get_argument(i));
    }

    use_tuple(tup, total() + 2);
  }

  Object* Arguments::shift(STATE) {
    Object* first = arguments_[0];

    if(argument_container_) {
      Tuple* tup = Tuple::create(state, total() - 1);
      for(uint32_t i = 1; i < total_; i++) {
        tup->put(state, i - 1, get_argument(i));
      }

      use_tuple(tup, total_ - 1);
    } else {
      total_--;
      arguments_++;
    }

    return first;
  }

  void Arguments::update_argument_container(Tuple* tup) {
    arguments_ = tup->field;
    argument_container_ = tup;
  }
}
