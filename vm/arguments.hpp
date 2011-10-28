#ifndef RBX_ARGUMENTS_HPP
#define RBX_ARGUMENTS_HPP

#include <stdint.h>
#include "builtin/array.hpp"
#include "builtin/tuple.hpp"

namespace rubinius {
  class Array;

  class GarbageCollector;

  class Arguments {
    Symbol* name_;
    Object* recv_;
    Object* block_;

    uint32_t total_;
    Object** arguments_;
    Tuple* argument_container_;

  public:
    Arguments(Symbol* name, Object* recv, Object* block, uint32_t total, Object** buffer)
      : name_(name)
      , recv_(recv)
      , block_(block)
      , total_(total)
      , arguments_(buffer)
      , argument_container_(0)
    {}

    Arguments(Symbol* name, Object* recv, uint32_t total, Object** buffer)
      : name_(name)
      , recv_(recv)
      , block_(Qnil)
      , total_(total)
      , arguments_(buffer)
      , argument_container_(0)
    {}

    Arguments(Symbol* name, uint32_t total, Object** buffer)
      : name_(name)
      , recv_(Qnil)
      , block_(Qnil)
      , total_(total)
      , arguments_(buffer)
      , argument_container_(0)
    {}

    Arguments(Symbol* name)
      : name_(name)
      , recv_(Qnil)
      , block_(Qnil)
      , total_(0)
      , arguments_(0)
      , argument_container_(0)
    {}

    Arguments(Symbol* name, Array* ary)
      : name_(name)
      , recv_(Qnil)
      , block_(Qnil)
    {
      use_array(ary);
    }

    Symbol* name() {
      return name_;
    }

    void set_name(Symbol* n) {
      name_ = n;
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

    void use_argument(Object* obj) {
      arguments_[0] = obj;
    }

    void use_array(Array* ary) {
      use_tuple(ary->tuple(), ary->size());
    }

    void use_tuple(Tuple* tup, int size) {
      total_ = size;
      arguments_ = tup->field;
      argument_container_ = tup;
    }

    Tuple*& argument_container_location() {
      return argument_container_;
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
