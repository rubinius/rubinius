#include "vm/test/test.hpp"

#include "compiled_file.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/symbol.hpp"

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

    CompiledFile* cf = CompiledFile::load(stream);
    TS_ASSERT_EQUALS(cf->magic, std::string("!RBIX"));
    TS_ASSERT_EQUALS(cf->signature, 1ULL);
    TS_ASSERT_EQUALS(cf->version, 42);
    TS_ASSERT_EQUALS((long)stream.tellg(), 11);
  }

  void test_body() {
    std::istringstream stream;
    stream.str("!RBIX\n1\n42\nt");

    CompiledFile* cf = CompiledFile::load(stream);
    TS_ASSERT_EQUALS(cf->body(state), Qtrue);
  }

  void test_load_file() {
    std::fstream stream("vm/test/fixture.rbc_");
    TS_ASSERT(!!stream);

    CompiledFile* cf = CompiledFile::load(stream);
    TS_ASSERT_EQUALS(cf->magic, "!RBIX");

    CompiledMethod* cm = try_as<CompiledMethod>(cf->body(state));
    TS_ASSERT(cm);
  }
};
