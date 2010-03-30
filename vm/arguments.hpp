#ifndef RBX_ARGUMENTS_HPP
#define RBX_ARGUMENTS_HPP

#include <stdint.h>
#include "builtin/array.hpp"
#include "builtin/tuple.hpp"

namespace rubinius {
  class Array;

  class GarbageCollector;

  class Arguments {
    Object* recv_;
    Object* block_;

    uint32_t total_;
    Object** arguments_;
    Tuple* argument_container_;

  public:
    Arguments(Object* recv, Object* block, uint32_t total, Object** buffer)
      : recv_(recv)
      , block_(block)
      , total_(total)
      , arguments_(buffer)
      , argument_container_(0)
    {}

    Arguments(Object* recv, uint32_t total, Object** buffer)
      : recv_(recv)
      , block_(Qnil)
      , total_(total)
      , arguments_(buffer)
      , argument_container_(0)
    {}

    Arguments(uint32_t total, Object** buffer)
      : recv_(Qnil)
      , block_(Qnil)
      , total_(total)
      , arguments_(buffer)
      , argument_container_(0)
    {}

    Arguments()
      : recv_(Qnil)
      , block_(Qnil)
      , total_(0)
      , arguments_(0)
      , argument_container_(0)
    {}

    Arguments(Array* ary)
      : recv_(Qnil)
      , block_(Qnil)
    {
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

    Object** arguments() {
      return arguments_;
    }

    Tuple* argument_container() {
      return argument_container_;
    }

    void update_argument_container(Tuple* obj);

    uint32_t total() {
      return total_;
    }

    void use_array(Array* ary) {
      use_tuple(ary->tuple(), ary->size());
    }

    void use_tuple(Tuple* tup, int size) {
      total_ = size;
      arguments_ = tup->field;
      argument_container_ = tup;
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
