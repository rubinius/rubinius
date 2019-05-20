#ifndef RBX_MARSHAL_HPP
#define RBX_MARSHAL_HPP

#include <iostream>
#include <sstream>

#include "defines.hpp"

namespace rubinius {

  class Object;

  class InstructionSequence;
  class CompiledCode;
  class String;
  class Encoding;
  class Array;
  class Bignum;
  class Float;
  class Symbol;
  class Tuple;

  class Marshaller {
  public:
    STATE;
    CompiledCode* code;
    std::ostream& stream;

    Marshaller(STATE, CompiledCode* code, std::ostream& stream)
      : state(state)
      , code(code)
      , stream(stream)
    {
    }

    void marshal();
    void marshal(Object* obj);
    void put_compiled_code(CompiledCode* code);
    void put_fixnum(Object* obj);
    void put_string(const char* type, String* str);
    void put_rational(Object* obj);
    void put_complex(Object* obj);
    void put_constant(Object* obj);
  };

  class UnMarshaller {
  public:
    STATE;
    std::istream& stream;

    UnMarshaller(STATE, std::istream& stream)
      : state(state)
      , stream(stream)
    {
    }

    Object* unmarshal();

    Object* get_int();
    Object* get_rational();
    Object* get_complex();
    String* get_string();
    Symbol* get_symbol();
    Tuple* get_tuple();

    Float* get_float();
    InstructionSequence* get_iseq();
    CompiledCode* get_compiled_code();
    Object* get_constant();
    Object* get_encoding();

  public:
    class Error {
      const char* message_;

    public:
      Error(const char* msg)
        : message_(msg)
      {}

      const char* message() const {
        return message_;
      }
    };
  };
}

#endif
