#include "machine/test/test.hpp"

#include "compiled_file.hpp"
#include "class/class.hpp"
#include "class/compiled_code.hpp"
#include "class/lookup_table.hpp"
#include "class/lexical_scope.hpp"
#include "class/symbol.hpp"

#include <cxxtest/TestSuite.h>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace rubinius;

class TestCompiledFile : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_load() {
    std::istringstream stream;
    stream.str("!RBIX\n1\n42\nt");

    CompiledFile* cf = CompiledFile::load(state, stream);
    TS_ASSERT_EQUALS(cf->magic, std::string("!RBIX"));
    TS_ASSERT_EQUALS(cf->signature, 1ULL);
    TS_ASSERT_EQUALS(cf->version, 42);
    TS_ASSERT_EQUALS((long)stream.tellg(), 11);
  }

  void test_body() {
    std::istringstream stream;
    stream.str("!RBIX\n1\n42\nt");

    CompiledFile* cf = CompiledFile::load(state, stream);
    TS_ASSERT_EQUALS(cf->body(state), cTrue);
  }

  void test_load_file() {
    std::fstream stream("machine/test/fixture.rbc_");
    TS_ASSERT(!!stream);

    CompiledFile* cf = CompiledFile::load(state, stream);
    TS_ASSERT_EQUALS(cf->magic, "!RBIX");

    CompiledCode* code = try_as<CompiledCode>(cf->body(state));
    TS_ASSERT(code);
  }
};
