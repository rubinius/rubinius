#include "vm/test/test.hpp"

#include "builtin/module.hpp"

class TestModule : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    Module* m = Module::create(state);
    TS_ASSERT_EQUALS(m->name(), Qnil);
    TS_ASSERT_EQUALS(m->superclass(), Qnil);
    TS_ASSERT(kind_of<LookupTable>(m->constants()));
    TS_ASSERT(kind_of<MethodTable>(m->method_table()));
  }
};
