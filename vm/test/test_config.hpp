#include "config_parser.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestConfig : public CxxTest::TestSuite {
  public:


  void setUp() {
  }

  void tearDown() {
  }

  void test_parse_line() {
    ConfigParser cfg;

    ConfigParser::Entry* e = cfg.parse_line("rbx.blah = 8");
    TS_ASSERT_EQUALS(std::string("rbx.blah"), e->variable);
    TS_ASSERT_EQUALS(std::string("8"), e->value);

    // try again with trailing whitespace
    e = cfg.parse_line("rbx.blah = 8 \t \t");
    TS_ASSERT_EQUALS(std::string("rbx.blah"), e->variable);
    TS_ASSERT_EQUALS(std::string("8"), e->value);
  }

  void test_parse_line_with_negative() {
    ConfigParser cfg;

    ConfigParser::Entry* e = cfg.parse_line("rbx.blah = -3");
    TS_ASSERT_EQUALS(std::string("rbx.blah"), e->variable);
    TS_ASSERT_EQUALS(std::string("-3"), e->value);
  }

  void test_parse_stream() {
    std::istringstream stream;

    stream.str("rbx.blah = 8\nrbx.foo = fun\n");

    ConfigParser cfg;

    cfg.import_stream(stream);

    ConfigParser::Entry* e = cfg.find("rbx.blah");
    TS_ASSERT(e);
    TS_ASSERT_EQUALS(e->variable, "rbx.blah");
    TS_ASSERT_EQUALS(e->value, "8");

    e = cfg.find("rbx.foo");
    TS_ASSERT(e);
    TS_ASSERT_EQUALS(e->variable, "rbx.foo");
    TS_ASSERT_EQUALS(e->value, "fun");
  }

  void test_is_number() {
    ConfigParser::Entry* ent = new ConfigParser::Entry();
    ent->value = std::string("blah");

    TS_ASSERT(!ent->is_number());

    ent->value = std::string("8");

    TS_ASSERT(ent->is_number());

    ent->value = std::string("-8");

    TS_ASSERT(ent->is_number());
  }

  void test_get_section() {
    std::istringstream stream;

    stream.str("rbx.test.blah = 8\nrbx.test.foo = fun\nrbx.crazy = true");

    ConfigParser cfg;

    cfg.import_stream(stream);

    ConfigParser::EntryList* l = cfg.get_section("rbx.test");

    TS_ASSERT_EQUALS(l->size(), (unsigned int)2);
    TS_ASSERT_EQUALS(l->at(0)->variable, "rbx.test.blah");
    TS_ASSERT_EQUALS(l->at(1)->variable, "rbx.test.foo");

    delete l;
  }

};
