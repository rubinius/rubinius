#include "machine/test/test.hpp"

#include "class/class.hpp"
#include "machine/object_utils.hpp"
#include "class/constant_table.hpp"
#include "class/method_table.hpp"
#include "object_types.hpp"

class TestClass : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    Class* c = Class::create(state, G(object));

    TS_ASSERT_EQUALS(c->module_name(), cNil);
    TS_ASSERT_EQUALS(c->superclass(), G(object));
    TS_ASSERT_EQUALS(c->instance_type(), G(object)->instance_type());
    TS_ASSERT(kind_of<ConstantTable>(c->constant_table()));
    TS_ASSERT(kind_of<MethodTable>(c->method_table()));
  }
};
