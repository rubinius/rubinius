#include "arguments.hpp"
#include "builtin/array.hpp"

namespace rubinius {
  void Arguments::append(STATE, Array* ary) {
    Array* args = Array::create(state, ary->size() + total());

    for(uint32_t i = 0; i < total(); i++) {
      args->set(state, i, get_argument(i));
    }

    for(uint32_t i = 0, n = total(); i < ary->size(); i++, n++) {
      args->set(state, n, ary->get(state, i));
    }

    use_array(args);
  }

  void Arguments::prepend(STATE, Array* ary) {
    Array* args = Array::create(state, ary->size() + total());

    for(uint32_t i = 0; i < ary->size(); i++) {
      args->set(state, i, ary->get(state, i));
    }

    for(uint32_t i = 0, n = ary->size(); i < total(); i++, n++) {
      args->set(state, n, get_argument(i));
    }

    use_array(args);
  }

  Array* Arguments::as_array(STATE) {
    Array* ary = Array::create(state, total());

    for(uint32_t i = 0; i < total(); i++) {
      ary->set(state, i, get_argument(i));
    }

    return ary;
  }

  void Arguments::unshift(STATE, Object* val) {
    if(array_) {
      array_->unshift(state, val);

      // Repoint internal things since we manipulated the array
      use_array(array_);
      return;
    }

    Array* ary = Array::create(state, total() + 1);

    ary->set(state, 0, val);

    for(uint32_t i = 0; i < total(); i++) {
      ary->set(state, i + 1, get_argument(i));
    }

    use_array(ary);
  }

  void Arguments::unshift2(STATE, Object* one, Object* two) {
    if(array_) {
      array_->unshift(state, two);
      array_->unshift(state, one);

      use_array(array_);
      return;
    }

    Array* ary = Array::create(state, total() + 2);

    ary->set(state, 0, one);
    ary->set(state, 1, two);

    for(uint32_t i = 0; i < total(); i++) {
      ary->set(state, i + 2, get_argument(i));
    }

    use_array(ary);
  }

  Object* Arguments::shift(STATE) {
    total_--;
    if(array_) {
      Object* first = array_->shift(state);

      use_array(array_);

      return first;
    } else {
      Object* first = arguments_[0];
      arguments_++;
      return first;
    }
  }
}
