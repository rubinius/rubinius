#include "builtin/class.hpp"
#include "builtin/lookuptable.hpp"
#include "object_types.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestClass : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_class_fields() {
    TS_ASSERT_EQUALS(Module::fields + 3U, Class::fields);
  }

  void test_metaclass_fields() {
    TS_ASSERT_EQUALS(Class::fields + 1U, MetaClass::fields);
  }

  void test_create() {
    Class* c = Class::create(state, G(object));

    TS_ASSERT_EQUALS(c->name(), Qnil);
    TS_ASSERT_EQUALS(c->superclass(), G(object));
    TS_ASSERT_EQUALS(c->instance_type(), G(object)->instance_type());
    TS_ASSERT_EQUALS(c->instance_fields(), G(object)->instance_fields());
    TS_ASSERT(kind_of<LookupTable>(c->constants()));
    TS_ASSERT(kind_of<LookupTable>(c->method_table()));
  }
};
