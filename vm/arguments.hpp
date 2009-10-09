#ifndef RBX_ARGUMENTS_HPP
#define RBX_ARGUMENTS_HPP

#include <stdint.h>
#include "builtin/array.hpp"
#include "builtin/tuple.hpp"

namespace rubinius {
  class Array;

  class Arguments {
    Object* recv_;
    Object* block_;

    uint32_t total_;
    Object** arguments_;

  public:
    Arguments(Object* recv, Object* block, uint32_t total, Object** buffer)
      : recv_(recv)
      , block_(block)
      , total_(total)
      , arguments_(buffer)
    {}

    Arguments(Object* recv, uint32_t total, Object** buffer)
      : recv_(recv)
      , block_(Qnil)
      , total_(total)
      , arguments_(buffer)
    {}

    Arguments(uint32_t total, Object** buffer)
      : recv_(0)
      , block_(Qnil)
      , total_(total)
      , arguments_(buffer)
    {}

    Arguments()
      : total_(0)
      , arguments_(0)
    {}

    Arguments(Array* ary) {
      use_array(ary);
    }

    Object* recv() {
      return recv_;
    }

    void set_recv(Object* val) {
      recv_ = val;
    }

    Object* block() {
      return block_;
    }

    void set_block(Object* val) {
      block_ = val;
    }

    Object* get_argument(uint32_t which) {
      return arguments_[which];
    }

    uint32_t total() {
      return total_;
    }

    void use_array(Array* ary) {
      total_ = ary->size();
      arguments_ = ary->tuple()->field + ary->start()->to_native();
    }

    void use_tuple(Tuple* tup, int size) {
      total_ = size;
      arguments_ = tup->field;
    }

    Array* as_array(STATE);
    void append(STATE, Array* ary);
    void prepend(STATE, Array* ary);

    void unshift(STATE, Object* val);
    void unshift2(STATE, Object* val, Object* val2);

    Object* shift(STATE);
  };
}

#endif
