#include "compiled_file.hpp"

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <sstream>

using namespace rubinius;

class TestCompiledFile : public CxxTest::TestSuite {
public:
#undef state
  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_load() {
    std::istringstream stream;
    stream.str("!RBIX\n1\naoeu\nt");

    CompiledFile* cf = CompiledFile::load(stream);
    TS_ASSERT_EQUALS(cf->magic, std::string("!RBIX"));
    TS_ASSERT_EQUALS(cf->version, 1);
    TS_ASSERT_EQUALS(cf->sum, std::string("aoeu"));
    TS_ASSERT_EQUALS((long)stream.tellg(), 13);
  }

  void test_body() {
    std::istringstream stream;
    stream.str("!RBIX\n1\naoeu\nt");

    CompiledFile* cf = CompiledFile::load(stream);
    TS_ASSERT_EQUALS(cf->body(state), Qtrue);
  }
};
