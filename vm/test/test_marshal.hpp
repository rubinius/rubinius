#include "marshal.hpp"

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <sstream>

using namespace rubinius;

class StringMarshaller : public Marshaller {
public:
  std::ostringstream sstream;

  StringMarshaller(STATE) : Marshaller(state, sstream) { }
};

class TestMarshal : public CxxTest::TestSuite {
public:
#undef state
  VM* state;
  StringMarshaller* mar;

  void setUp() {
    state = new VM();
    mar = new StringMarshaller(state);
  }

  void tearDown() {
    delete state;
    delete mar;
  }

  void test_int() {
    mar->marshal(Object::i2n(1));
    TS_ASSERT_EQUALS(mar->sstream.str(), "I\n1\n");
  }

  void test_bignum() {
    mar->marshal(Bignum::create(state, 1));
    TS_ASSERT_EQUALS(mar->sstream.str(), "I\n1\n");
  }

  void test_string() {
    String* str = String::create(state, "blah");
    mar->marshal(str);
    TS_ASSERT_EQUALS(mar->sstream.str(), "s\n4\nblah\n");
  }

  void test_string_with_null() {
    String* str = String::create(state, "blah\0more", 9);
    mar->marshal(str);
    TS_ASSERT_EQUALS(mar->sstream.str(), std::string("s\n9\nblah\0more\n", 14));
  }

  void test_symbol() {
    mar->marshal(state->symbol("blah"));
    TS_ASSERT_EQUALS(mar->sstream.str(), "x\nblah\n");
  }

  void test_sendsite() {
    mar->marshal(SendSite::create(state, state->symbol("blah")));
    TS_ASSERT_EQUALS(mar->sstream.str(), "S\nblah\n");
  }

  void test_array() {
    Array* ary = Array::create(state, 3);
    ary->set(state, 0, Object::i2n(1));
    ary->set(state, 1, Object::i2n(2));
    ary->set(state, 2, Object::i2n(3));

    mar->marshal(ary);
    TS_ASSERT_EQUALS(mar->sstream.str(), std::string("A\n3\nI\n1\nI\n2\nI\n3\n"));
  }

  void test_array_with_inner_array() {
    Array* ary = Array::create(state, 3);
    ary->set(state, 0, Object::i2n(1));
    ary->set(state, 1, Object::i2n(2));
    ary->set(state, 2, Object::i2n(3));

    Array* out = Array::create(state, 2);
    out->set(state, 0, ary);
    out->set(state, 1, Object::i2n(4));

    mar->marshal(out);
    TS_ASSERT_EQUALS(mar->sstream.str(), std::string("A\n2\nA\n3\nI\n1\nI\n2\nI\n3\nI\n4\n"));
  }

  void test_tuple() {
    Tuple* tup = Tuple::from(state, 2, Object::i2n(8), Object::i2n(10));

    mar->marshal(tup);
    TS_ASSERT_EQUALS(mar->sstream.str(), std::string("p\n2\nI\n8\nI\n10\n"));
  }

  void test_float() {
    mar->marshal(Float::create(state, 15.5));
    TS_ASSERT_EQUALS(mar->sstream.str(), std::string("d\n15.5\n"));
  }

};
