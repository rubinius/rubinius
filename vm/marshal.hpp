#ifndef RBX_MARSHAL_HPP
#define RBX_MARSHAL_HPP

#include <iostream>
#include <sstream>

#include "prelude.hpp"

namespace rubinius {

  class Object;

  class SendSite;
  class InstructionSequence;
  class CompiledMethod;
  class String;
  class Array;
  class Bignum;
  class Float;
  class Symbol;
  class Tuple;

  class Marshaller {
  public:
    STATE;
    std::ostream& stream;

    Marshaller(STATE, std::ostream& stream) : state(state), stream(stream) { }

    void marshal(Object* obj);

    void set_int(Object* o);
    void set_string(String* o);
    void set_symbol(Symbol* o);
    void set_sendsite(SendSite* o);
    void set_array(Array* o);
    void set_tuple(Tuple* o);
    void set_bignum(Bignum* o);
    void set_float(Float* o);
    void set_iseq(InstructionSequence* o);
    void set_cmethod(CompiledMethod* o);
  };

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
    SendSite* get_sendsite();
    Array* get_array();
    Tuple* get_tuple();

    Float* get_float();
    InstructionSequence* get_iseq();
    CompiledMethod* get_cmethod();

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
