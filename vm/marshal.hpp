#ifndef RBX_MARSHAL_HPP
#define RBX_MARSHAL_HPP

#include <iostream>
#include <sstream>

#include "prelude.hpp"

namespace rubinius {

  class Object;

  class InstructionSequence;
  class CompiledMethod;
  class String;
  class Array;
  class Bignum;
  class Float;
  class Symbol;
  class Tuple;

  class UnMarshaller {
  public:
    STATE;
    std::istream& stream;

    UnMarshaller(STATE, std::istream& stream) :
      state(state), stream(stream) { }

    Object* unmarshal();

    Object* get_int();
    String* get_string();
    Symbol* get_symbol();
    Tuple* get_tuple();

    Float* get_float();
    InstructionSequence* get_iseq();
    CompiledMethod* get_cmethod();
    Object* get_constant();

  public:
    class Error {
      const char* message_;

    public:
      Error(const char* msg)
        : message_(msg)
      {}

      const char* message() {
        return message_;
      }
    };
  };
}

#endif
