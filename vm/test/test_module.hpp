#include "builtin/module.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestModule : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    Module* m = Module::create(state);
    TS_ASSERT_EQUALS(m->name(), Qnil);
    TS_ASSERT_EQUALS(m->superclass(), Qnil);
    TS_ASSERT_EQUALS(m->constants()->obj_type, LookupTableType);
    TS_ASSERT(kind_of<LookupTable>(m->method_table()));
  }
};
