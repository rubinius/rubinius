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
    TS_ASSERT_EQUALS(m->module_name(), cNil);
    TS_ASSERT_EQUALS(m->superclass(), cNil);
    TS_ASSERT(kind_of<ConstantTable>(m->constant_table()));
    TS_ASSERT(kind_of<MethodTable>(m->method_table()));
  }
};
