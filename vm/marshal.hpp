#ifndef RBX_MARSHAL_HPP
#define RBX_MARSHAL_HPP

#include <iostream>
#include <sstream>

#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"

/* not sure whats up with this, but it's needed. */
#undef state

namespace rubinius {

  class Marshaller {
  public:
    STATE;
    std::ostream& stream;

    Marshaller(STATE, std::ostream& stream) : state(state), stream(stream) { }

    void marshal(OBJECT obj);

    void set_int(OBJECT o);
    void set_string(String* o);
    void set_symbol(SYMBOL o);
    void set_sendsite(SendSite* o);
    void set_array(Array* o);
    void set_tuple(Tuple* o);
    void set_bignum(Bignum* o);
    void set_float(Float* o);
    void set_iseq(ISeq* o);
    void set_cmethod(CompiledMethod* o);
  };

  class UnMarshaller {
  public:
    STATE;
    std::istream& stream;

    UnMarshaller(STATE, std::istream& stream) :
      state(state), stream(stream) { }

    OBJECT unmarshal();

    OBJECT get_int();
    String* get_string();
    SYMBOL get_symbol();
    SendSite* get_sendsite();
    Array* get_array();
    Tuple* get_tuple();

    Float* get_float();
    ISeq* get_iseq();
    CompiledMethod* get_cmethod();
  };
}

#endif
