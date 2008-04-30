#include "marshal.hpp"

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <sstream>

using namespace rubinius;

class StringUnMarshaller : public UnMarshaller {
public:
  std::istringstream sstream;

  StringUnMarshaller(STATE) : UnMarshaller(state, sstream) { }
};

class TestUnMarshal : public CxxTest::TestSuite {
public:
#undef state
  VM* state;
  StringUnMarshaller* mar;

  void setUp() {
    state = new VM();
    mar = new StringUnMarshaller(state);
  }

  void tearDown() {
    delete state;
    delete mar;
  }

  void test_init() {
    mar->sstream.str(std::string("I\n3\n"));
    OBJECT obj = mar->unmarshal();

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->n2i(), 3);
  }

  void test_string() {
    mar->sstream.str(std::string("s\n4\nblah\n"));
    OBJECT obj = mar->unmarshal();

    TS_ASSERT(kind_of<String>(obj));
    String *str = as<String>(obj);
    TS_ASSERT_EQUALS(std::string(*str), "blah");
  }

  void test_symbol() {
    mar->sstream.str(std::string("x\nblah\n"));
    OBJECT obj = mar->unmarshal();

    TS_ASSERT(obj->symbol_p());
    TS_ASSERT_EQUALS(obj, state->symbol("blah"));
  }
  
  void test_sendsite() {
    mar->sstream.str(std::string("S\nblah\n"));
    OBJECT obj = mar->unmarshal();

    TS_ASSERT(kind_of<SendSite>(obj));
    TS_ASSERT_EQUALS(as<SendSite>(obj)->name, state->symbol("blah"));
  }

  void test_array() {
    mar->sstream.str(std::string("A\n3\nI\n1\nx\nfoo\ns\n3\nblah\n"));
    OBJECT obj = mar->unmarshal();

    TS_ASSERT(kind_of<Array>(obj));
    
    Array* ary = as<Array>(obj);

    TS_ASSERT_EQUALS(ary->get(state, 0), Object::i2n(1));
    TS_ASSERT_EQUALS(ary->get(state, 1), state->symbol("foo"));

    String* str = as<String>(ary->get(state, 2));
    TS_ASSERT_EQUALS(std::string("blah"), (char*)*str);
  }

  void test_tuple() {
    mar->sstream.str(std::string("p\n2\nI\n2\nI\n47\n"));

    OBJECT obj = mar->unmarshal();

    TS_ASSERT(kind_of<Tuple>(obj));

    Tuple* tup = as<Tuple>(obj);

    TS_ASSERT_EQUALS(tup->at(0), Object::i2n(2));
    TS_ASSERT_EQUALS(tup->at(1), Object::i2n(47));
  }

  void test_float() {
    mar->sstream.str(std::string("d\n15.5\n"));

    OBJECT obj = mar->unmarshal();

    TS_ASSERT(kind_of<Float>(obj));

    Float* flt = as<Float>(obj);

    TS_ASSERT_EQUALS(flt->val, 15.5);
  }

};
